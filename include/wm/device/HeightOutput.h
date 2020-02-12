#pragma once

#include "wm/Device.h"

#include <SM_Vector.h>

namespace wm
{
namespace device
{

class HeightOutput : public Device
{
public:
    HeightOutput()
    {
        m_imports = {
            {{ DeviceVarType::Heightfield, "in" }},
        };
    }

    virtual void Execute(const Context& ctx) override;

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "wm/device/HeightOutput.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // HeightOutput

}
}