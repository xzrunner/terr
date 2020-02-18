#pragma once

#include "wm/Device.h"

namespace wm
{
namespace device
{

class LayoutGen : public Device
{
public:
    LayoutGen()
    {
        m_exports = {
            {{ DeviceVarType::Heightfield, "out" }},
        };
    }

    virtual void Execute() override;

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "wm/device/LayoutGen.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // LayoutGen

}
}