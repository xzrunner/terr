#include "wm/device/Inverter.h"
#include "wm/HeightField.h"
#include "wm/DeviceHelper.h"

namespace wm
{
namespace device
{

void Inverter::Execute()
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

    m_hf = std::make_shared<HeightField>(w, h);
    m_hf->SetValues(vals);
}

}
}