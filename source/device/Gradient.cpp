#include "wm/device/Gradient.h"
#include "wm/HeightField.h"

#include <sm_const.h>
#include <SM_Calc.h>

namespace wm
{
namespace device
{

void Gradient::Execute()
{
    std::vector<float> vals(m_width * m_height);
    const sm::vec2 center(m_width * 0.5f, m_height * 0.5f);
    const float rad = m_dir * SM_DEG_TO_RAD;
    const float hw = m_width * 0.5f;
    for (size_t y = 0; y < m_height; ++y) {
        for (size_t x = 0; x < m_width; ++x) {
            auto new_pos = sm::rotate_vector(sm::vec2(x, y) - center, m_dir * SM_DEG_TO_RAD) + center;
            const float h = 0.5f - (new_pos.x - hw) / hw * 0.5f;
            vals[y * m_width + x] = h;
        }
    }

    m_hf = std::make_shared<HeightField>(m_width, m_height);
    m_hf->SetValues(vals);
}

}
}