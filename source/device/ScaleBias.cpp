#include "terraingraph/device/ScaleBias.h"
#include "terraingraph/DeviceHelper.h"

#include <heightfield/HeightField.h>

namespace terraingraph
{
namespace device
{

void ScaleBias::Execute(const std::shared_ptr<dag::Context>& ctx)
{
    auto prev_hf = DeviceHelper::GetInputHeight(*this, 0);
    if (!prev_hf) {
        return;
    }

    auto w = prev_hf->Width();
    auto h = prev_hf->Height();
    m_hf = std::make_shared<hf::HeightField>(w, h);
    std::vector<float> vals(w * h);

    auto& p_vals = prev_hf->GetValues();
    assert(p_vals.size() == w * h);
    for (size_t i = 0, n = p_vals.size(); i < n; ++i) {
        vals[i] = p_vals[i] * m_scale + m_bias;
    }
    m_hf->SetValues(vals);
}

}
}