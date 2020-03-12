#include "terraingraph/device/NormalMap.h"
#include "terraingraph/DeviceHelper.h"
#include "terraingraph/HemanHelper.h"
#include "terraingraph/EvalGPU.h"

#include <heightfield/HeightField.h>
#include <unirender/Blackboard.h>
#include <unirender/RenderContext.h>
#include <painting0/ShaderUniforms.h>

#include <heman.h>

namespace
{

std::shared_ptr<terraingraph::EvalGPU> EVAL = nullptr;

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

uniform sampler2D u_heightmap;

uniform float u_scale;
uniform vec2 u_inv_res;

vec3 ComputeNormalCentralDifference(vec2 position, float scale)
{
    float left   = texture(u_heightmap, position - vec2(1.0, 0.0) * u_inv_res).r * scale;
    float right  = texture(u_heightmap, position + vec2(1.0, 0.0) * u_inv_res).r * scale;
    float bottom = texture(u_heightmap, position - vec2(0.0, 1.0) * u_inv_res).r * scale;
    float top    = texture(u_heightmap, position + vec2(0.0, 1.0) * u_inv_res).r * scale;
    return normalize(vec3(left - right, 2.0, bottom - top));
}

vec3 ComputeNormalSobelFilter(vec2 position, float scale)
{
    float upper_left   = texture(u_heightmap, position + vec2(-1.0,  1.0) * u_inv_res).r * scale;
    float upper_center = texture(u_heightmap, position + vec2( 0.0,  1.0) * u_inv_res).r * scale;
    float upper_right  = texture(u_heightmap, position + vec2( 1.0,  1.0) * u_inv_res).r * scale;
    float left         = texture(u_heightmap, position + vec2(-1.0,  0.0) * u_inv_res).r * scale;
    float right        = texture(u_heightmap, position + vec2( 1.0,  0.0) * u_inv_res).r * scale;
    float lower_left   = texture(u_heightmap, position + vec2(-1.0, -1.0) * u_inv_res).r * scale;
    float lower_center = texture(u_heightmap, position + vec2( 0.0, -1.0) * u_inv_res).r * scale;
    float lower_right  = texture(u_heightmap, position + vec2( 1.0, -1.0) * u_inv_res).r * scale;

    float x = upper_right + (2.0 * right) + lower_right - upper_left - (2.0 * left) - lower_left;
    float y = lower_left + (2.0 * lower_center) + lower_right - upper_left - (2.0 * upper_center) - upper_right;

    return normalize(vec3(-x, 1.0, y));
}

void main(void)
{
	vec3 normal = ComputeNormalSobelFilter(fs_in.texcoord, u_scale);
    FragColor = vec4(normal * 0.5 + 0.5, 1.0);
}

)";

}

namespace terraingraph
{
namespace device
{

void NormalMap::Execute(const std::shared_ptr<dag::Context>& ctx)
{
    auto prev_hf = DeviceHelper::GetInputHeight(*this, 0);
    if (!prev_hf) {
        return;
    }

#ifdef NORMAL_MAP_GPU
    RunGPU(*prev_hf);
#else
    RunCPU(*prev_hf);
#endif // NORMAL_MAP_GPU
}

void NormalMap::Init()
{
    if (!EVAL)
    {
        auto& rc = ur::Blackboard::Instance()->GetRenderContext();

        std::vector<std::string> texture_names;
        texture_names.push_back("u_heightmap");

        EVAL = std::make_shared<EvalGPU>(rc, vs, fs, texture_names);
    }
}

void NormalMap::RunGPU(const hf::HeightField& hf)
{
    size_t w = hf.Width();
    size_t h = hf.Height();

    auto& rc = ur::Blackboard::Instance()->GetRenderContext();

    std::vector<uint32_t> textures;
    auto heightmap = hf.GetHeightmap();
    textures.push_back(heightmap->TexID());

    pt0::ShaderUniforms vals;
    vals.AddVar("u_scale", pt0::RenderVariant(m_scale));
    sm::vec2 inv_res(1.0f / heightmap->Width(), 1.0f / heightmap->Height());
    vals.AddVar("u_inv_res", pt0::RenderVariant(inv_res));

    m_bmp = std::make_shared<Bitmap>(w, h);
    EVAL->RunPS(rc, textures, vals, *m_bmp);
}

void NormalMap::RunCPU(const hf::HeightField& hf)
{
    size_t w = hf.Width();
    size_t h = hf.Height();

    auto he_height = HemanHelper::Encode(hf);

    auto he_norm = heman_lighting_compute_normals(he_height);
    auto he_norm_data = heman_image_data(he_norm);

    m_bmp = std::make_shared<Bitmap>(w, h);

    auto norm_data = m_bmp->GetPixels();
    for (size_t i = 0, n = w * h * 3; i < n; ++i) {
        const float flt01 = (he_norm_data[i] + 1) * 0.5f;
        norm_data[i] = static_cast<unsigned char>(flt01 * 255);
    }

    heman_image_destroy(he_height);
    heman_image_destroy(he_norm);
}

}
}