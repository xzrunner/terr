#pragma once

#include "terraingraph/Device.h"
#include "terraingraph/NoiseQuality.h"

namespace terraingraph
{
namespace device
{

class Billow : public Device
{
public:
    Billow()
    {
        m_exports = {
            {{ DeviceVarType::Heightfield, "out" }},
        };
    }

    virtual void Execute() override;

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "terraingraph/device/Billow.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Billow

}
}