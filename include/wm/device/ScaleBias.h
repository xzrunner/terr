#pragma once

#include "wm/Device.h"
#include "wm/NoiseQuality.h"

namespace wm
{
namespace device
{

class ScaleBias : public Device
{
public:
    ScaleBias()
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

#define PARM_FILEPATH "wm/device/ScaleBias.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // ScaleBias

}
}