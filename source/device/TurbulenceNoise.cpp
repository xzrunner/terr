#include "terraingraph/device/TurbulenceNoise.h"
#include "terraingraph/HeightField.h"
#include "terraingraph/EvalGPU.h"

#include <unirender/Blackboard.h>
#include <unirender/RenderContext.h>
#include <painting0/ShaderUniforms.h>

namespace
{

std::shared_ptr<terraingraph::EvalGPU> EVAL = nullptr;

int PERLIN_PERM_TEXID = 0;
int PERLIN_GRAD_TEXID = 0;

// Original Ken Perlin's permutation table
const uint8_t PERLIN_PERM_TABLE[256]={151,160,137,91,90,15,
	131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
	190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
	88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,134,139,48,27,166,
	77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
	102,143,54,65,25,63,161,255,216,80,73,209,76,132,187,208,89,18,169,200,196,
	135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,250,124,123,
	5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
	223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,43,172,9,
	129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,228,
	251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,107,
	49,192,214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,
	138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180};

// ----------------------------------------------------------------------------
// 2D off-axis gradients table
// (128*(1+cos(angle)), 128*(1+sin(angle)) pairs for
// eight angles: 22.5, 67.5, 112.5, 157.5, 202.5, 247.5, 292.5, 337.5
const uint8_t PERLIN_GRAD_TABLE[16] = {
    245, 176,
    176, 245,
    79, 245,
    10, 176,
    10, 79,
    79, 10,
    176, 10,
    245, 79 };

template<class T>
inline T lerp(const T &src, const T &dest, float blendFactor)
{
    return src + (dest - src) * blendFactor;
}

const char* vs = R"(

#version 330 core
layout (location = 0) in vec4 position;
layout (location = 1) in vec2 texcoord;

out VS_OUT {
    vec2 texcoord;
} vs_out;

void main()
{
	vs_out.texcoord = texcoord;
	gl_Position = position;
}

)";

const char* fs = R"(

#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec2 texcoord;
} fs_in;


uniform sampler2D perlin_perm2d;
uniform sampler2D perlin_grad2d;

#define OCTAVES 12 // using a 'static' value here to prevent the need for dynamic loop branching

uniform int noise_type;

uniform mat4 noise_octave_amps;
uniform mat4 distort_octave_amps;

uniform vec4 noise_trans;

uniform float uv_scale;
uniform float seed;
uniform float lacunarity;
uniform float gain;

uniform float distortion_scale;
uniform float distortion_gain;
uniform float distortion_uv_scale;
uniform float warp;
uniform float gain1;
uniform float warp0;
uniform float damp0;
uniform float damp;
uniform float damp_scale;

float PerlinNoise(vec2 p, float seed)
{
	// Calculate 2D integer coordinates i and fraction p.
	vec2 i = floor(p);
	vec2 f = p - i;

	// Get weights from the coordinate fraction
	vec2 w = f * f * f * (f * (f * 6 - 15) + 10);
	vec4 w4 = vec4(1, w.x, w.y, w.x * w.y);

	// Get the four randomly permutated indices from the noise lattice nearest to
	// p and offset these numbers with the seed number.
	vec4 perm = texture2D(perlin_perm2d, i / 256) + seed;

	// Permutate the four offseted indices again and get the 2D gradient for each
	// of the four permutated coordinates-seed pairs.
	vec4 g1 = texture2D(perlin_grad2d, perm.xy) * 2 - 1;
	vec4 g2 = texture2D(perlin_grad2d, perm.zw) * 2 - 1;

	// Evaluate the four lattice gradients at p
	float a = dot(g1.xy, f);
	float b = dot(g2.xy, f + vec2(-1,  0));
	float c = dot(g1.zw, f + vec2( 0, -1));
	float d = dot(g2.zw, f + vec2(-1, -1));

	// Bi-linearly blend between the gradients, using w4 as blend factors.
	vec4 grads = vec4(a, b - a, c - a, a - b - c + d);
	float n = dot(grads, w4);

	// Return the noise value, roughly normalized in the range [-1, 1]
	return n * 1.5;
}

float Turbulence(vec2 p, float seed, int octaves, float lacunarity, float gain)
{
	float sum = 0;
	float freq = 1.0, amp = 1.0;
	for (int i=0; i < octaves; i++)
	{
		float n = PerlinNoise(p*freq, seed + i / 256.0);
		sum += n*amp;
		freq *= lacunarity;
		amp *= gain;
	}
	return sum;
}

float BillowedNoise(vec2 p, float seed)
{
	return abs(PerlinNoise(p, seed));
}

float BillowyTurbulence(vec2 p, float seed, int octaves, float lacunarity, float gain)
{
	float sum = 0;
	float freq = 1.0, amp = 1.0;
	for (int i=0; i < octaves; i++)
	{
		float n = BillowedNoise(p*freq, seed + i / 256.0);
		sum += n*amp;
		freq *= lacunarity;
		amp *= gain;
	}
	return sum;
}

float RidgedNoise(vec2 p, float seed)
{
	return 1.0f-abs(PerlinNoise(p, seed));
}

float RidgedTurbulence(vec2 p, float seed, int octaves, float lacunarity, float gain)
{
	float sum = 0;
	float freq = 1.0, amp = 1.0;
	for (int i=0; i < octaves; i++)
	{
		float n = RidgedNoise(p*freq, seed + i / 256.0);
		sum += n*amp;
		freq *= lacunarity;
		amp *= gain;
	}
	return sum;
}

vec3 PerlinNoisePseudoDeriv(vec2 p, float seed)
{
	// Calculate 2D integer coordinates i and fraction p.
	vec2 i = floor(p);
	vec2 f = p - i;

	// Get weights from the coordinate fraction
	vec2 w = f * f * f * (f * (f * 6 - 15) + 10);
	vec4 w4 = vec4(1, w.x, w.y, w.x * w.y);

	// Get pseudo derivative weights
	vec2 dw = f * f * (f * (30 * f - 60) + 30);

	// Get the four randomly permutated indices from the noise lattice nearest to
	// p and offset these numbers with the seed number.
	vec4 perm = texture2D(perlin_perm2d, i / 256) + seed;

	// Permutate the four offseted indices again and get the 2D gradient for each
	// of the four permutated coordinates-seed pairs.
	vec4 g1 = texture2D(perlin_grad2d, perm.xy) * 2 - 1;
	vec4 g2 = texture2D(perlin_grad2d, perm.zw) * 2 - 1;

	// Evaluate the four lattice gradients at p
	float a = dot(g1.xy, f);
	float b = dot(g2.xy, f + vec2(-1,  0));
	float c = dot(g1.zw, f + vec2( 0, -1));
	float d = dot(g2.zw, f + vec2(-1, -1));

	// Bi-linearly blend between the gradients, using w4 as blend factors.
	vec4 grads = vec4(a, b - a, c - a, a - b - c + d);
	float n = dot(grads, w4);

	// Calculate pseudo derivates
	float dx = dw.x * (grads.y + grads.w*w.y);
	float dy = dw.y * (grads.z + grads.w*w.x);

	// Return the noise value, roughly normalized in the range [-1, 1]
	// Also return the pseudo dn/dx and dn/dy, scaled by the same factor
	return vec3(n, dx, dy) * 1.5;
}

float IqTurbulence(vec2 p, float seed, int octaves, float lacunarity, float gain)
{
	float sum = 0.5;
	float freq = 1.0, amp = 1.0;
	vec2 dsum = vec2(0,0);
	for (int i=0; i < octaves; i++)
	{
		vec3 n = PerlinNoisePseudoDeriv(p*freq, seed + i / 256.0);
		dsum += n.yz;
		sum += amp * n.x / (1 + dot(dsum, dsum));
		freq *= lacunarity;
		amp *= gain;
	}
	return sum;
}

vec3 PerlinNoiseDeriv(vec2 p, float seed)
{
	// Calculate 2D integer coordinates i and fraction p.
	vec2 i = floor(p);
	vec2 f = p - i;

	// Get weights from the coordinate fraction
	vec2 w = f * f * f * (f * (f * 6 - 15) + 10); // 6f^5 - 15f^4 + 10f^3
	vec4 w4 = vec4(1, w.x, w.y, w.x * w.y);

	// Get the derivative dw/df
	vec2 dw = f * f * (f * (f * 30 - 60) + 30); // 30f^4 - 60f^3 + 30f^2

	// Get the derivative d(w*f)/df
	vec2 dwp = f * f * f * (f * (f * 36 - 75) + 40); // 36f^5 - 75f^4 + 40f^3

	// Get the four randomly permutated indices from the noise lattice nearest to
	// p and offset these numbers with the seed number.
	vec4 perm = texture2D(perlin_perm2d, i / 256) + seed;

	// Permutate the four offseted indices again and get the 2D gradient for each
	// of the four permutated coordinates-seed pairs.
	vec4 g1 = texture2D(perlin_grad2d, perm.xy) * 2 - 1;
	vec4 g2 = texture2D(perlin_grad2d, perm.zw) * 2 - 1;

	// Evaluate the four lattice gradients at p
	float a = dot(g1.xy, f);
	float b = dot(g2.xy, f + vec2(-1,  0));
	float c = dot(g1.zw, f + vec2( 0, -1));
	float d = dot(g2.zw, f + vec2(-1, -1));

	// Bi-linearly blend between the gradients, using w4 as blend factors.
	vec4 grads = vec4(a, b - a, c - a, a - b - c + d);
	float n = dot(grads, w4);

	// Calculate the derivatives dn/dx and dn/dy
	float dx = (g1.x + (g1.z-g1.x)*w.y) + ((g2.y-g1.y)*f.y - g2.x + ((g1.y-g2.y-g1.w+g2.w)*f.y + g2.x +
	           g1.w - g2.z - g2.w)*w.y)*dw.x + ((g2.x-g1.x) + (g1.x-g2.x-g1.z+g2.z)*w.y)*dwp.x;
	float dy = (g1.y + (g2.y-g1.y)*w.x) + ((g1.z-g1.x)*f.x - g1.w + ((g1.x-g2.x-g1.z+g2.z)*f.x + g2.x +
	           g1.w - g2.z - g2.w)*w.x)*dw.y + ((g1.w-g1.y) + (g1.y-g2.y-g1.w+g2.w)*w.x)*dwp.y;

	return vec3(n, dx, dy) * 1.5;
}

vec3 NoiseDeriv(vec2 p, float seed)
{
    vec2 P = floor(p);
    p -= P;
    vec2 w = p * p * p * (p * (p * 6 - 15) + 10); // 6p^5 - 15p^4 + 10p^3
    vec2 dw = p * p * (p * (p * 30 - 60) + 30); // 30p^4 - 60p^3 + 30p^2
    vec2 dwp = p * p * p * (p * (p * 36 - 75) + 40); // 36p^5 - 75p^4 + 40p^3

    vec4 AA = texture2D(perlin_perm2d, P / 256) + seed / 256;
    vec4 G1 = texture2D(perlin_grad2d, AA.xy) * 2 - 1;
    vec4 G2 = texture2D(perlin_grad2d, AA.zw) * 2 - 1;

    float k0 = G1.x*p.x + G1.y*p.y; // a
    float k1 = (G2.x-G1.x)*p.x + (G2.y-G1.y)*p.y - G2.x; // b - a
    float k2 = (G1.z-G1.x)*p.x + (G1.w-G1.y)*p.y - G1.w; // c - a
    float k3 = (G1.x-G2.x-G1.z+G2.z)*p.x + (G1.y-G2.y-G1.w+G2.w)*p.y + G2.x + G1.w - G2.z - G2.w; // a - b - c + d

    float n = k0 + k1*w.x + k2*w.y + k3*w.x*w.y;

    float dx = (G1.x + (G1.z-G1.x)*w.y) + ((G2.y-G1.y)*p.y - G2.x + ((G1.y-G2.y-G1.w+G2.w)*p.y + G2.x + G1.w - G2.z - G2.w)*w.y)*dw.x + ((G2.x-G1.x) + (G1.x-G2.x-G1.z+G2.z)*w.y)*dwp.x;
    float dy = (G1.y + (G2.y-G1.y)*w.x) + ((G1.z-G1.x)*p.x - G1.w + ((G1.x-G2.x-G1.z+G2.z)*p.x + G2.x + G1.w - G2.z - G2.w)*w.x)*dw.y + ((G1.w-G1.y) + (G1.y-G2.y-G1.w+G2.w)*w.x)*dwp.y;

    return vec3(n, dx, dy) * 1.5;
}

float SwissTurbulence(vec2 p, float seed, int octaves,
                      float lacunarity, float gain,
					  float warp)
{
     float sum = 0;
     float freq = 1.0, amp = 1.0;
     vec2 dsum = vec2(0,0);
     for(int i=0; i < octaves; i++)
     {
         vec3 n = PerlinNoiseDeriv((p + warp * dsum)*freq, seed + i / 256.0);
         sum += amp * (1 - abs(n.x));
         dsum += amp * n.yz * -n.x;
         freq *= lacunarity;
         amp *= gain * clamp(sum, 0.0, 1.0);
    }
    return sum;
}

float JordanTurbulence(vec2 p, float seed, int octaves, float lacunarity,
                       float gain1, float gain,
                       float warp0, float warp,
                       float damp0, float damp,
                       float damp_scale)
{
    vec3 n = PerlinNoiseDeriv(p, seed);
    vec3 n2 = n * n.x;
    float sum = n2.x;
    vec2 dsum_warp = warp0*n2.yz;
    vec2 dsum_damp = damp0*n2.yz;

    float amp = gain1;
    float freq = lacunarity;
    float damped_amp = amp * gain;

    for(int i=1; i < octaves; i++)
    {
        n = PerlinNoiseDeriv(p * freq + dsum_warp.xy, seed + i / 256.0);
        n2 = n * n.x;
        sum += damped_amp * n2.x;
        dsum_warp += warp * n2.yz;
        dsum_damp += damp * n2.yz;
        freq *= lacunarity;
        amp *= gain;
        damped_amp = amp * (1-damp_scale/(1+dot(dsum_damp,dsum_damp)));
    }
    return sum;
}

float Erosion1Turbulence(vec2 p, vec2 pdistort, mat4 octaveAmps, float seed, int octaves, float lacunarity)
{
    float sum = 0;
    float freq = 1.0;
    vec2 dsum = vec2(0,0);
    float amp2 = 1;
    float minus = 0;
    for(int i=0; i < octaves; i++)
    {
        float amp = amp2 * octaveAmps[i/4][i%4];
//      vec3 n = NoiseDeriv((p + pdistort*amp * clamp(2*sum, 0.0, 1.0)+ 0.15 * dsum)*freq, seed + i);
        vec3 n = NoiseDeriv((p + 0.15 * dsum)*freq, seed + i);
        n.yz *= -n.x; //Clamp(n.x * -2, -1, 1);
        n.x = 1 - abs(n.x);
        dsum += amp * n.yz;
        sum += amp * n.x;
        freq *= lacunarity;
        amp2 *= clamp(sum, 0.0, 1.0) / 0.9;
    }
    return sum - 0.6;
}

float Erosion2Turbulence(vec2 p, vec2 pdistort, mat4 octaveAmps, float seed, int octaves, float lacunarity)
{
    vec3 sum = vec3(0.5, 0.0, 0.0);
    float freq = 1.0;
    vec2 dsum = vec2(0,0);
    vec2 dsumo = dsum;
    float amp2 = 1.0f;
    for(int i=0; i < octaves; i++)
    {
        float amp = amp2 * octaveAmps[i/4][i%4];
        vec3 n = NoiseDeriv((p)*freq, seed + i);
        vec3 n2 = NoiseDeriv((p)*freq + 0.01 * n.yz, seed + i);

        n.x += 5 * (n.x - n2.x) * (n.x - n2.x);
        n.x *= exp(-dot(n.yz, n.yz));

        dsumo = dsum;
        dsum += amp * n.yz;
        sum += amp * n;
        freq *= lacunarity;
        amp2 *= (1 + 0.02 * dot(n.yz, n.yz));
    }
    return sum.x;
}

float psPerlinNoise()
{
	float height = PerlinNoise(fs_in.texcoord * uv_scale, seed);
	return height * 0.5f + 0.5f;
}

float psTurbulence()
{
	float height = Turbulence(fs_in.texcoord * uv_scale, seed, 10, 4, 0.5);
	return height * 0.4f + 0.5f;
}

float psBillowyTurbulence()
{
	float height = BillowyTurbulence(fs_in.texcoord * uv_scale, seed, OCTAVES, lacunarity, gain);
	return height * 0.5f + 0.25f;
}

float psRidgedTurbulence()
{
	float height = RidgedTurbulence(fs_in.texcoord * uv_scale, seed, OCTAVES, lacunarity, gain);
	return height * 0.5f - 0.33f;
}

float psIqTurbulence()
{
	float height = IqTurbulence(fs_in.texcoord * uv_scale, seed, OCTAVES, lacunarity, gain);
	return height * 0.4f + 0.33f;
}

float psSwissTurbulence()
{
	vec2 p = fs_in.texcoord * uv_scale;
	p.x += distortion_scale * Turbulence(fs_in.texcoord * distortion_uv_scale, seed + 0.1, 4, lacunarity, distortion_gain);
	p.y += distortion_scale * Turbulence(fs_in.texcoord * distortion_uv_scale, seed + 0.2, 4, lacunarity, distortion_gain);
	float height = SwissTurbulence(p, seed, OCTAVES, lacunarity, gain, warp);
	return 0.25f * height - 0.05f;
}

float psJordanTurbulence()
{
	vec2 p = fs_in.texcoord * uv_scale;
	p.x += distortion_scale * Turbulence(fs_in.texcoord * distortion_uv_scale, seed + 0.1, 4, lacunarity, distortion_gain);
	p.y += distortion_scale * Turbulence(fs_in.texcoord * distortion_uv_scale, seed + 0.2, 4, lacunarity, distortion_gain);
	float height = JordanTurbulence(p, seed, OCTAVES, lacunarity, gain1, gain, warp0, warp, damp0, damp, damp_scale);
	return height;
}

float psErosion1Turbulence()
{
    vec2 p = fs_in.texcoord * uv_scale;
    //vec2 p = noise_trans.xy * fs_in.texcoord.x + noise_trans.zw * fs_in.texcoord.y;
	float height = Erosion1Turbulence(p, vec2(0, 0), noise_octave_amps, seed, OCTAVES, lacunarity);
	return height;
}

float psErosion2Turbulence()
{
    vec2 p = fs_in.texcoord * uv_scale;
    //vec2 p = noise_trans.xy * fs_in.texcoord.x + noise_trans.zw * fs_in.texcoord.y;
	float height = Erosion2Turbulence(p, vec2(0, 0), noise_octave_amps, seed, OCTAVES, lacunarity);
	return height;
}

void main()
{
    float v = 0;
    switch (noise_type)
    {
    case 0:
        v = psPerlinNoise();
        break;
    case 1:
        v = psTurbulence();
        break;
    case 2:
        v = psBillowyTurbulence();
        break;
    case 3:
        v = psRidgedTurbulence();
        break;
    case 4:
        v = psIqTurbulence();
        break;
    case 5:
        v = psSwissTurbulence();
        break;
    case 6:
        v = psJordanTurbulence();
        break;
    case 7:
        v = psErosion1Turbulence();
        break;
    case 8:
        v = psErosion2Turbulence();
        break;
    }

    if (noise_type == 7 || noise_type == 8)
    {
        v = v * 0.25;

        const float noise_blend = 1.0;
        const float noise_peakify = 1.0;
        v = mix(1.0f, pow(0.5 + v, noise_peakify) - pow(0.5, noise_peakify), noise_blend);
    }

    v = clamp(v, 0.0, 1.0);
	FragColor = vec4(v, v, v, 1.0);
}

)";


}

namespace terraingraph
{
namespace device
{

void TurbulenceNoise::Execute()
{
    m_hf = std::make_shared<HeightField>(m_width, m_height);

    std::vector<uint32_t> textures;
    textures.push_back(PERLIN_PERM_TEXID);
    textures.push_back(PERLIN_GRAD_TEXID);

    pt0::ShaderUniforms vals;

    sm::mat4 noise_octave_amps;
    sm::mat4 distort_octave_amps;
    CalcOctaveAmps(noise_octave_amps, distort_octave_amps);
    vals.AddVar("noise_octave_amps",   pt0::RenderVariant(noise_octave_amps));
    vals.AddVar("distort_octave_amps", pt0::RenderVariant(distort_octave_amps));

    float noise_stretch_angle = SM_DEG_TO_RAD * m_noise_stretch_angle;
    vals.AddVar("noise_trans", pt0::RenderVariant(sm::vec4(
         cos(noise_stretch_angle) / m_noise_largest_feat * sqrtf(m_noise_stretch_ratio),
         sin(noise_stretch_angle) / m_noise_largest_feat / sqrtf(m_noise_stretch_ratio),
        -sin(noise_stretch_angle) / m_noise_largest_feat * sqrtf(m_noise_stretch_ratio),
         cos(noise_stretch_angle) / m_noise_largest_feat / sqrtf(m_noise_stretch_ratio)
    )));

    vals.AddVar("noise_type", pt0::RenderVariant(static_cast<int>(m_type)));

    vals.AddVar("uv_scale",   pt0::RenderVariant(m_uv_scale));
    vals.AddVar("seed",       pt0::RenderVariant(m_seed));
    vals.AddVar("lacunarity", pt0::RenderVariant(m_lacunarity));
    vals.AddVar("gain",       pt0::RenderVariant(m_gain));

    vals.AddVar("distortion_scale",    pt0::RenderVariant(m_distortion_scale));
    vals.AddVar("distortion_gain",     pt0::RenderVariant(m_distortion_gain));
    vals.AddVar("distortion_uv_scale", pt0::RenderVariant(m_distortion_uv_scale));
    vals.AddVar("warp",                pt0::RenderVariant(m_warp));
    vals.AddVar("gain1",               pt0::RenderVariant(m_gain1));
    vals.AddVar("warp0",               pt0::RenderVariant(m_warp0));
    vals.AddVar("damp0",               pt0::RenderVariant(m_damp0));
    vals.AddVar("damp",                pt0::RenderVariant(m_damp));
    vals.AddVar("damp_scale",          pt0::RenderVariant(m_damp_scale));

    auto& rc = ur::Blackboard::Instance()->GetRenderContext();
    EVAL->RunPS(rc, textures, vals, *m_hf);
}

void TurbulenceNoise::Init()
{
    if (PERLIN_PERM_TEXID == 0 || PERLIN_GRAD_TEXID == 0) {
        InitLookupTextures();
    }
    if (!EVAL) {
        InitEval();
    }
}

void TurbulenceNoise::InitLookupTextures()
{
    if (PERLIN_PERM_TEXID != 0 && PERLIN_GRAD_TEXID != 0) {
        return;
    }

    auto& rc = ur::Blackboard::Instance()->GetRenderContext();

    uint32_t pixels[256 * 256];
    memset(pixels, 0, sizeof(uint32_t) * 256 * 256);
    uint32_t* ptr = &pixels[0];

	// create optimized noise permutation table
	// r = perm(perm(x    ) + (y))
	// g = perm(perm(x    ) + (y + 1))
	// b = perm(perm(x + 1) + (y))
	// a = perm(perm(x + 1) + (y + 1))
	for (int y = 0; y < 256; ++y)
	{
		for (int x = 0; x < 256; ++x)
		{
			uint8_t a = PERLIN_PERM_TABLE[ x           ] + (uint8_t)(y);
            uint8_t b = PERLIN_PERM_TABLE[(x + 1) & 255] + (uint8_t)(y);
			*ptr ++=	(PERLIN_PERM_TABLE[(b + 1) & 255] << 24) +
						(PERLIN_PERM_TABLE[ b           ] << 16) +
						(PERLIN_PERM_TABLE[(a + 1) & 255] << 8) +
						(PERLIN_PERM_TABLE[ a           ]);
		}
	}

    PERLIN_PERM_TEXID = rc.CreateTexture(pixels, 256, 256, ur::TEXTURE_RGBA8);

    memset(pixels, 0, sizeof(uint32_t) * 256 * 256);
    ptr = &pixels[0];
	// create optimized noise gradient-permutation table
	// r = grad(perm(x) & 7).x
	// g = grad(perm(x) & 7).y
	// b = grad(perm(y) & 7).x
	// a = grad(perm(y) & 7).y
	for (int y = 0; y < 256; ++y)
	{
		for (int x = 0; x < 256; ++x)
		{
			uint8_t px = PERLIN_PERM_TABLE[x];
            uint8_t py = PERLIN_PERM_TABLE[y];
			*ptr++ =	(PERLIN_GRAD_TABLE[((py & 7) << 1) + 1] << 24) +
						(PERLIN_GRAD_TABLE[((py & 7) << 1)    ] << 16) +
						(PERLIN_GRAD_TABLE[((px & 7) << 1) + 1] << 8) +
						(PERLIN_GRAD_TABLE[((px & 7) << 1)    ]);
		}
	}

    PERLIN_GRAD_TEXID = rc.CreateTexture(pixels, 256, 256, ur::TEXTURE_RGBA8);
}

void TurbulenceNoise::InitEval()
{
    auto& rc = ur::Blackboard::Instance()->GetRenderContext();

    std::vector<std::string> texture_names;
    texture_names.push_back("perlin_perm2d");
    texture_names.push_back("perlin_grad2d");

    EVAL = std::make_shared<EvalGPU>(rc, vs, fs, texture_names);
}

void TurbulenceNoise::CalcOctaveAmps(sm::mat4& noise_octave_amps,
                                     sm::mat4& distort_octave_amps)
{
	float noiseHighestOctave = log(m_noise_largest_feat) / log(2.0f);
	float distortHighestOctave = log(m_distort_largest_feat) / log(2.0f);

	float scale = 1.0f;
	for (int index = 0; index < 16; ++index)
	{
        noise_octave_amps.c[index / 4][index % 4] = scale;
		scale *= 0.5f * lerp(m_noise_coarse_rough, m_noise_fine_rough, std::min(1.0f, index / noiseHighestOctave));
	}

	scale = 1.0f;
	for (int index = 0; index < 16; ++index)
	{
        distort_octave_amps.c[index / 4][index % 4] = scale;
		scale *= 0.5f * lerp(m_distort_coarse_rough, m_distort_fine_rough, std::min(1.0f, index / distortHighestOctave));
	}
}

}
}