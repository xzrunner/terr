#pragma once

#include "wm/Device.h"

namespace wm
{
namespace device
{

class Inverter : public Device
{
public:
    Inverter()
    {
        m_imports = {
            {{ DeviceVarType::Heightfield, "in" }},
        };
        m_exports = {
            {{ DeviceVarType::Heightfield, "out" }},
        };
    }

    virtual void Execute() override;

private:
    void Init();

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "wm/device/Inverter.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Inverter

}
}