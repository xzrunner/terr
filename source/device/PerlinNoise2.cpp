#include "terraingraph/device/PerlinNoise2.h"

#include <heightfield/HeightField.h>
#include <heightfield/Utility.h>

#include <noise/module/perlin.h>

namespace terraingraph
{
namespace device
{

void PerlinNoise2::Execute(const std::shared_ptr<dag::Context>& ctx)
{
    m_hf = std::make_shared<hf::HeightField>(m_width, m_height);

    noise::NoiseQuality quality;
    switch (m_quality)
    {
    case NoiseQuality::Fast:
        quality = noise::NoiseQuality::QUALITY_FAST;
        break;
    case NoiseQuality::Standard:
        quality = noise::NoiseQuality::QUALITY_STD;
        break;
    case NoiseQuality::Best:
        quality = noise::NoiseQuality::QUALITY_BEST;
        break;
    default:
        assert(0);
    }

    noise::module::Perlin noise;
    noise.SetNoiseQuality(quality);
    noise.SetFrequency(m_frequency);
    noise.SetLacunarity(m_lacunarity);
    noise.SetOctaveCount(m_octave_count);
    noise.SetPersistence(m_persistence);
    noise.SetSeed(m_seed);

    for (size_t y = 0; y < m_height; ++y) {
        for (size_t x = 0; x < m_width; ++x) {
            const float fx = static_cast<float>(x) / m_width;
            const float fy = static_cast<float>(y) / m_height;
            double v = noise.GetValue(fx, 0, fy);
            m_hf->Set(x, y, hf::Utility::HeightDoubleToShort(v));
        }
    }
}

}
}