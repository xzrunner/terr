#include "terraingraph/device/Clamp.h"
#include "terraingraph/DeviceHelper.h"

#include <heightfield/HeightField.h>
#include <heightfield/Utility.h>

namespace terraingraph
{
namespace device
{

void Clamp::Execute(const std::shared_ptr<dag::Context>& ctx)
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
    int32_t min = hf::Utility::HeightFloatToShort(m_lower_bound);
    int32_t max = hf::Utility::HeightFloatToShort(m_upper_bound);
    for (size_t i = 0, n = p_vals.size(); i < n; ++i) {
        vals[i] = std::min(std::max(p_vals[i], min), max);
    }
    m_hf->SetValues(vals);
}

}
}