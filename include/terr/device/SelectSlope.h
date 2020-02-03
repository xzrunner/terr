#pragma once

#include "terr/Device.h"

#include <unirender/Texture.h>

namespace terr
{
namespace device
{

class SelectSlope : public Device
{
public:
    SelectSlope()
    {
        m_imports = {
            {{ DeviceVarType::Heightfield, "in" }},
        };
        m_exports = {
            {{ DeviceVarType::Heightfield, "out" }},
        };
    }

    virtual void Execute(const Context& ctx) override;

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "terr/device/SelectSlope.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // SelectSlope

}
}