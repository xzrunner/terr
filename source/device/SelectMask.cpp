#include "wm/device/SelectMask.h"
#include "wm/DeviceHelper.h"
#include "wm/HeightField.h"
#include "wm/Mask.h"

namespace wm
{
namespace device
{

void SelectMask::Execute()
{
    auto prev_hf = DeviceHelper::GetInputHeight(*this, ID_SOURCE);
    if (!prev_hf) {
        return;
    }

    auto prev_mask = DeviceHelper::GetInputMask(*this, ID_MASK);
    if (!prev_mask) {
        return;
    }

    auto w = prev_hf->Width();
    auto h = prev_hf->Height();
    if (prev_mask->Width() != w ||
        prev_mask->Height() != h) {
        return;
    }

    m_hf = std::make_shared<HeightField>(w, h);
    std::vector<float> vals(w * h);

    auto& h_vals = prev_hf->GetValues();
    auto& m_vals = prev_mask->GetValues();
    assert(h_vals.size() == w * h
        && m_vals.size() == w * h);
    for (size_t i = 0, n = h_vals.size(); i < n; ++i)
    {
        if (m_vals[i]) {
            vals[i] = h_vals[i];
        } else {
            vals[i] = 0;
        }
    }
    m_hf->SetValues(vals);
}

}
}