#include "terraingraph/device/PerlinNoise.h"
#include "terraingraph/PerlinNoise.h"

#include <heightfield/HeightField.h>
#include <heightfield/Utility.h>

namespace terraingraph
{
namespace device
{

void PerlinNoise::Execute(const std::shared_ptr<dag::Context>& ctx)
{
    m_hf = std::make_shared<hf::HeightField>(m_width, m_height);

    terraingraph::PerlinNoise noise;

    for (size_t y = 0; y < m_height; ++y) {
        for (size_t x = 0; x < m_width; ++x) {
            Vec3f p;
            p.x = static_cast<float>(x) / m_width;
            p.y = static_cast<float>(y) / m_height;
            p.z = 0;
            m_hf->Set(x, y, hf::Utility::HeightFloatToShort(noise.eval(p)));
        }
    }

    AddFractalPattern(noise);
}

void PerlinNoise::AddFractalPattern(const terraingraph::PerlinNoise& noise)
{
    if (!m_hf || m_fractal_layer == 0) {
        return;
    }

    for (uint32_t j = 0; j < m_hf->Height(); ++j)
    {
        for (uint32_t i = 0; i < m_hf->Width(); ++i)
        {
            float fractal = 0;
            float amplitude = 1;
            Vec3f pt = Vec3f((float)i, (float)j, 0) * (1 / 128.f);
            for (int k = 0; k < m_fractal_layer; ++k) {
                fractal += noise.eval(pt) * amplitude;
                pt *= 2;
                amplitude *= 0.5f;
            }
            m_hf->Set(i, j, hf::Utility::HeightFloatToShort(fractal));
        }
    }
}

}
}