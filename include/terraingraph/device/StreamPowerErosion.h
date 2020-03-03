#pragma once

#include "terraingraph/Device.h"

namespace terraingraph
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

    virtual void Execute(const std::shared_ptr<dag::Context>& ctx = nullptr) override;

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "terraingraph/device/StreamPowerErosion.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // StreamPowerErosion

}
}