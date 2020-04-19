#include "terraingraph/device/ScaleBias.h"
#include "terraingraph/DeviceHelper.h"
#include "terraingraph/Context.h"

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
    std::vector<int32_t> vals(w * h);

    auto& dev = *std::static_pointer_cast<Context>(ctx)->ur_dev;
    auto& p_vals = prev_hf->GetValues(dev);
    assert(p_vals.size() == w * h);
    for (size_t i = 0, n = p_vals.size(); i < n; ++i) {
        vals[i] = static_cast<int32_t>(p_vals[i] * m_scale + m_bias);
    }
    m_hf->SetValues(vals);
}

}
}