#include "wm/device/Resize.h"
#include "wm/DeviceHelper.h"
#include "wm/Mask.h"

namespace wm
{
namespace device
{

void Resize::Execute()
{
    auto prev_mask = DeviceHelper::GetInputMask(*this, 0);
    if (!prev_mask) {
        return;
    }

    size_t prev_w = prev_mask->Width();
    size_t prev_h = prev_mask->Height();
    if (prev_w == m_width &&
        prev_h == m_height) {
        m_mask = std::make_shared<Mask>(*prev_mask);
        return;
    }

    auto& prev_vals = prev_mask->GetValues();

    const float sx = static_cast<float>(prev_w) / m_width;
    const float sy = static_cast<float>(prev_h) / m_height;
    std::vector<bool> vals(m_width * m_height, false);
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            size_t px = static_cast<size_t>(x * sx);
            size_t py = static_cast<size_t>(y * sy);
            vals[y * m_width + x] = prev_vals[py * prev_w + px];
        }
    }

    m_mask = std::make_shared<Mask>(m_width, m_height);
    m_mask->SetValues(vals);
}

}
}