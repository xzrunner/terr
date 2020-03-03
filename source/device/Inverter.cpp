#include "terraingraph/device/Inverter.h"
#include "terraingraph/DeviceHelper.h"

#include <heightfield/HeightField.h>

namespace terraingraph
{
namespace device
{

void Inverter::Execute(const std::shared_ptr<dag::Context>& ctx)
{
    auto prev_hf = DeviceHelper::GetInputHeight(*this, 0);
    if (!prev_hf) {
        return;
    }

    auto w = prev_hf->Width();
    auto h = prev_hf->Height();
    auto vals = prev_hf->GetValues();
    for (auto& v : vals) {
        v = 1.0f - v;
    }

    m_hf = std::make_shared<hf::HeightField>(w, h);
    m_hf->SetValues(vals);
}

}
}