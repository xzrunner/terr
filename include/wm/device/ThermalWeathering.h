#pragma once

#include "wm/Device.h"

namespace wm
{
namespace device
{

class ThermalWeathering : public Device
{
public:
    ThermalWeathering()
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
    void Step();

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "wm/device/ThermalWeathering.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // ThermalWeathering

}
}