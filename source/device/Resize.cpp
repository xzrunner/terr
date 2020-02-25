#include "wm/device/Resize.h"
#include "wm/DeviceHelper.h"
#include "wm/Mask.h"
#include "wm/HeightField.h"

namespace wm
{
namespace device
{

void Resize::Execute()
{
    auto prev_mask = DeviceHelper::GetInputMask(*this, 0);
    if (prev_mask) {
        ResizeMask(*prev_mask);
        return;
    }

    auto prev_hf = DeviceHelper::GetInputHeight(*this, 0);
    if (prev_hf) {
        ResizeHeightField(*prev_hf);
        return;
    }
}

void Resize::ResizeMask(const Mask& mask)
{
    size_t prev_w = mask.Width();
    size_t prev_h = mask.Height();
    if (prev_w == m_width &&
        prev_h == m_height) {
        m_mask = std::make_shared<Mask>(mask);
        return;
    }

    auto& prev_vals = mask.GetValues();

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

void Resize::ResizeHeightField(const HeightField& hf)
{
    size_t prev_w = hf.Width();
    size_t prev_h = hf.Height();
    if (prev_w == m_width &&
        prev_h == m_height) {
        m_hf = std::make_shared<HeightField>(hf);
        return;
    }

    auto& prev_vals = hf.GetValues();

    const float sx = static_cast<float>(prev_w) / m_width;
    const float sy = static_cast<float>(prev_h) / m_height;
    std::vector<float> vals(m_width * m_height, false);
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            size_t px = static_cast<size_t>(x * sx);
            size_t py = static_cast<size_t>(y * sy);
            vals[y * m_width + x] = prev_vals[py * prev_w + px];
        }
    }

    m_hf = std::make_shared<HeightField>(m_width, m_height);
    m_hf->SetValues(vals);
}

}
}