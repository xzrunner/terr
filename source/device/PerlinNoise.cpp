#include "terraingraph/device/PerlinNoise.h"
#include "terraingraph/PerlinNoise.h"

#include <heightfield/HeightField.h>

namespace terraingraph
{
namespace device
{

void PerlinNoise::Execute()
{
    m_hf = std::make_shared<hf::HeightField>(m_width, m_height);

    terraingraph::PerlinNoise noise;

    for (size_t y = 0; y < m_height; ++y) {
        for (size_t x = 0; x < m_width; ++x) {
            Vec3f p;
            p.x = static_cast<float>(x) / m_width;
            p.y = static_cast<float>(y) / m_height;
            p.z = 0;
            m_hf->Set(x, y, (noise.eval(p) + 1) * 0.5f);
        }
    }

    AddFractalPattern(noise);
}

void PerlinNoise::AddFractalPattern(const terraingraph::PerlinNoise& noise)
{
    if (!m_hf || m_fractal_layer == 0) {
        return;
    }

    float max_val = 0;
    for (uint32_t j = 0; j < m_hf->Height(); ++j)
    {
        for (uint32_t i = 0; i < m_hf->Width(); ++i)
        {
            float fractal = 0;
            float amplitude = 1;
            Vec3f pt = Vec3f((float)i, (float)j, 0) * (1 / 128.f);
            for (int k = 0; k < m_fractal_layer; ++k) {
                fractal += (1 + noise.eval(pt)) * 0.5f * amplitude;
                pt *= 2;
                amplitude *= 0.5f;
            }
            if (fractal > max_val) {
                max_val = fractal;
            }
            m_hf->Set(i, j, fractal);
        }
    }

    if (max_val != 0) {
        m_hf->Scale(1.0f / max_val);
    }
}

}
}