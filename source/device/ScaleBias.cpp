#include "terraingraph/device/ScaleBias.h"
#include "terraingraph/DeviceHelper.h"
#include "terraingraph/HeightField.h"

namespace terraingraph
{
namespace device
{

void ScaleBias::Execute()
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
    for (size_t i = 0, n = p_vals.size(); i < n; ++i) {
        vals[i] = p_vals[i] * m_scale + m_bias;
    }
    m_hf->SetValues(vals);
}

}
}