#include "terraingraph/device/Resample.h"
#include "terraingraph/DeviceHelper.h"
#include "terraingraph/Mask.h"

#include <heightfield/HeightField.h>

namespace terraingraph
{
namespace device
{

void Resample::Execute(const std::shared_ptr<dag::Context>& ctx)
{
    auto prev_mask = DeviceHelper::GetInputMask(*this, 0);
    if (prev_mask) {
        ResampleMask(*prev_mask);
        return;
    }

    auto prev_hf = DeviceHelper::GetInputHeight(*this, 0);
    if (prev_hf) {
        m_hf = ResampleHeightField(*prev_hf, m_width, m_height);
        return;
    }
}

std::shared_ptr<hf::HeightField>
Resample::ResampleHeightField(const hf::HeightField& hf, int width, int height)
{
    size_t prev_w = hf.Width();
    size_t prev_h = hf.Height();
    if (prev_w == width &&
        prev_h == height) {
        return std::make_shared<hf::HeightField>(hf);
    }

    auto& prev_vals = hf.GetValues();

    const float sx = static_cast<float>(prev_w) / width;
    const float sy = static_cast<float>(prev_h) / height;
    std::vector<int32_t> vals(width * height);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            size_t px = static_cast<size_t>(x * sx);
            size_t py = static_cast<size_t>(y * sy);
            vals[y * width + x] = prev_vals[py * prev_w + px];
        }
    }

    auto new_hf = std::make_shared<hf::HeightField>(width, height);
    new_hf->SetValues(vals);
    return new_hf;
}

void Resample::ResampleMask(const Mask& mask)
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
    for (size_t y = 0; y < m_height; ++y) {
        for (size_t x = 0; x < m_width; ++x) {
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