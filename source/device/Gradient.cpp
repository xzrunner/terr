#include "terraingraph/device/Gradient.h"

#include <sm_const.h>
#include <SM_Calc.h>
#include <heightfield/HeightField.h>
#include <heightfield/Utility.h>

namespace terraingraph
{
namespace device
{

void Gradient::Execute(const std::shared_ptr<dag::Context>& ctx)
{
    std::vector<int32_t> vals(m_width * m_height);
    const sm::vec2 center(m_width * 0.5f, m_height * 0.5f);
    const float rad = m_dir * SM_DEG_TO_RAD;
    const float hw = m_width * 0.5f;
    for (size_t y = 0; y < m_height; ++y)
    {
        for (size_t x = 0; x < m_width; ++x)
        {
            auto pos = sm::vec2(static_cast<float>(x), static_cast<float>(y));
            auto new_pos = sm::rotate_vector(pos - center, m_dir * SM_DEG_TO_RAD) + center;
            const float h = -(new_pos.x - hw) / hw;
            vals[y * m_width + x] = hf::Utility::HeightFloatToShort(h);
        }
    }

    m_hf = std::make_shared<hf::HeightField>(m_width, m_height);
    m_hf->SetValues(vals);
}

}
}