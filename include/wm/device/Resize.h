#pragma once

#include "wm/Device.h"

namespace wm
{
namespace device
{

class Resize : public Device
{
public:
    Resize()
    {
        m_imports = {
            {{ DeviceVarType::Any, "in" }},
        };
        m_exports = {
            {{ DeviceVarType::Any, "out" }},
        };
    }

    virtual void Execute() override;

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "wm/device/Resize.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Resize

}
}