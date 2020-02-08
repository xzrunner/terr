#pragma once

#include "wm/Device.h"

#include <unirender/Texture.h>

namespace wm
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

#define PARM_FILEPATH "wm/device/SelectSlope.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // SelectSlope

}
}