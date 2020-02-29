#pragma once

#include "terraingraph/Device.h"

namespace terraingraph
{
namespace device
{

class Rock : public Device
{
public:
    Rock()
    {
        m_exports = {
            {{ DeviceVarType::Heightfield, "out" }},
        };
    }

    virtual void Execute() override;

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "terraingraph/device/Rock.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Rock

}
}