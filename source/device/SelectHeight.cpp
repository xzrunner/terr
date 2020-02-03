#include "terr/device/SelectHeight.h"
#include "terr/DeviceHelper.h"
#include "terr/HeightField.h"

namespace terr
{
namespace device
{

void SelectHeight::Execute(const Context& ctx)
{
    auto prev_hf = DeviceHelper::GetInputHeight(*this, 0);
    if (!prev_hf) {
        return;
    }

    auto w = prev_hf->Width();
    auto h = prev_hf->Height();
    m_hf = std::make_shared<HeightField>(w, h);
    std::vector<float> vals(w * h);

    auto& p_vals = prev_hf->GetValues();
    assert(p_vals.size() == w * h);
    for (size_t i = 0, n = p_vals.size(); i < n; ++i)
    {
        if (p_vals[i] >= m_min && p_vals[i] <= m_max) {
            vals[i] = 1;
        } else {
            vals[i] = 0;
        }
    }
    m_hf->SetValues(vals);
}

}
}