#include "terraingraph/device/SelectMask.h"
#include "terraingraph/DeviceHelper.h"
#include "terraingraph/Mask.h"
#include "terraingraph/Context.h"

#include <heightfield/HeightField.h>

namespace terraingraph
{
namespace device
{

void SelectMask::Execute(const std::shared_ptr<dag::Context>& ctx)
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

    m_hf = std::make_shared<hf::HeightField>(w, h);
    std::vector<int32_t> vals(w * h);

    auto& dev = *std::static_pointer_cast<Context>(ctx)->ur_dev;
    auto& h_vals = prev_hf->GetValues(dev);
    auto mask_p = prev_mask->GetPixels();
    for (size_t i = 0, n = h_vals.size(); i < n; ++i)
    {
        if (mask_p[i]) {
            vals[i] = h_vals[i];
        } else {
            vals[i] = 0;
        }
    }
    m_hf->SetValues(vals);
}

}
}