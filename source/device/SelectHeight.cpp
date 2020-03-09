#include "terraingraph/device/SelectHeight.h"
#include "terraingraph/DeviceHelper.h"

#include <heightfield/HeightField.h>
#include <heightfield/Utility.h>

namespace terraingraph
{
namespace device
{

void SelectHeight::Execute(const std::shared_ptr<dag::Context>& ctx)
{
    auto prev_hf = DeviceHelper::GetInputHeight(*this, 0);
    if (!prev_hf) {
        return;
    }

    auto w = prev_hf->Width();
    auto h = prev_hf->Height();
    m_hf = std::make_shared<hf::HeightField>(w, h);
    std::vector<int32_t> vals(w * h);

    auto& p_vals = prev_hf->GetValues();
    assert(p_vals.size() == w * h);
    for (size_t i = 0, n = p_vals.size(); i < n; ++i)
    {
        if (p_vals[i] >= m_min && p_vals[i] <= m_max) {
            vals[i] = hf::Utility::HeightFloatToShort(1.0f);
        } else {
            vals[i] = 0;
        }
    }
    m_hf->SetValues(vals);
}

}
}