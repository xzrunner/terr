#pragma once

#include "wm/Device.h"

namespace wm
{
namespace device
{

class StreamPowerErosion : public Device
{
public:
    StreamPowerErosion()
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

#define PARM_FILEPATH "wm/device/StreamPowerErosion.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // StreamPowerErosion

}
}