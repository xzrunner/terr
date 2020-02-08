#include "wm/device/BasicNoise.h"
#include "wm/Context.h"
#include "wm/HeightField.h"

namespace wm
{
namespace device
{

void BasicNoise::Execute(const Context& ctx)
{
    m_hf = std::make_shared<HeightField>(ctx.size, ctx.size);

    for (size_t y = 0; y < ctx.size; ++y) {
        for (size_t x = 0; x < ctx.size; ++x) {
            m_hf->Set(x, y, GetHeight(x, y));
        }
    }
}

float BasicNoise::GetHeight(size_t x, size_t y) const
{
    return m_amplitude * Total(x, y);
}

float BasicNoise::Total(size_t x, size_t y) const
{
    //properties of one octave (changing each loop)
    float t = 0.0f;
    float _amplitude = 1;
    float freq = m_frequency;

    for(int i = 0; i < m_octaves; i++)
    {
        t += GetValue(y * freq + m_randomseed, x * freq + m_randomseed) * _amplitude;
        _amplitude *= m_persistence;
        freq *= 2;
    }

    return t;
}

float BasicNoise::GetValue(float x, float y) const
{
    int Xint = (int)x;
    int Yint = (int)y;
    float Xfrac = x - Xint;
    float Yfrac = y - Yint;

    //noise values
    float n01 = Noise(Xint-1, Yint-1);
    float n02 = Noise(Xint+1, Yint-1);
    float n03 = Noise(Xint-1, Yint+1);
    float n04 = Noise(Xint+1, Yint+1);
    float n05 = Noise(Xint-1, Yint);
    float n06 = Noise(Xint+1, Yint);
    float n07 = Noise(Xint, Yint-1);
    float n08 = Noise(Xint, Yint+1);
    float n09 = Noise(Xint, Yint);

    float n12 = Noise(Xint+2, Yint-1);
    float n14 = Noise(Xint+2, Yint+1);
    float n16 = Noise(Xint+2, Yint);

    float n23 = Noise(Xint-1, Yint+2);
    float n24 = Noise(Xint+1, Yint+2);
    float n28 = Noise(Xint, Yint+2);

    float n34 = Noise(Xint+2, Yint+2);

    //find the noise values of the four corners
    float x0y0 = 0.0625f*(n01+n02+n03+n04) + 0.125f*(n05+n06+n07+n08) + 0.25f*(n09);
    float x1y0 = 0.0625f*(n07+n12+n08+n14) + 0.125f*(n09+n16+n02+n04) + 0.25f*(n06);
    float x0y1 = 0.0625f*(n05+n06+n23+n24) + 0.125f*(n03+n04+n09+n28) + 0.25f*(n08);
    float x1y1 = 0.0625f*(n09+n16+n28+n34) + 0.125f*(n08+n14+n06+n24) + 0.25f*(n04);

    //interpolate between those values according to the x and y fractions
    float v1 = Interpolate(x0y0, x1y0, Xfrac); //interpolate in x direction (y)
    float v2 = Interpolate(x0y1, x1y1, Xfrac); //interpolate in x direction (y+1)
    float fin = Interpolate(v1, v2, Yfrac);  //interpolate in y direction

    return fin;
}

float BasicNoise::Interpolate(float x, float y, float a) const
{
    float negA = 1.0f - a;
    float negASqr = negA * negA;
    float fac1 = 3.0f * (negASqr) - 2.0f * (negASqr * negA);
    float aSqr = a * a;
    float fac2 = 3.0f * aSqr - 2.0f * (aSqr * a);

    return x * fac1 + y * fac2; //add the weighted factors
}

float BasicNoise::Noise(size_t x, size_t y) const
{
    int n = x + y * 57;
    n = (n << 13) ^ n;
    int t = (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff;
    return 1.0f - float(t * 0.931322574615478515625e-9);/// 1073741824.0f);
}

}
}