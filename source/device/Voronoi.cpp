#include "terraingraph/device/Voronoi.h"

#include <heightfield/HeightField.h>
#include <heightfield/Utility.h>

#include <noise/module/voronoi.h>

namespace terraingraph
{
namespace device
{

void Voronoi::Execute(const std::shared_ptr<dag::Context>& ctx)
{
    m_hf = std::make_shared<hf::HeightField>(m_width, m_height);

    noise::module::Voronoi noise;
    noise.SetDisplacement(m_displacement);
    noise.EnableDistance(m_enable_dist);
    noise.SetFrequency(m_frequency);
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