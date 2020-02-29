#pragma once

#include "terraingraph/Device.h"

namespace terraingraph
{
namespace device
{

class HydraulicErosion : public Device
{
public:
    HydraulicErosion()
    {
        m_imports = {
            {{ DeviceVarType::Heightfield, "in" }},
        };
        m_exports = {
            {{ DeviceVarType::Heightfield, "out" }},
        };
    }

    virtual void Execute() override;

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "terraingraph/device/HydraulicErosion.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // HydraulicErosion

}
}