#pragma once

#include "terraingraph/Device.h"
#include "terraingraph/NoiseQuality.h"

namespace terraingraph
{
namespace device
{

class PerlinNoise2 : public Device
{
public:
    PerlinNoise2()
    {
        m_exports = {
            {{ DeviceVarType::Heightfield, "out" }},
        };
    }

    virtual void Execute() override;

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "terraingraph/device/PerlinNoise2.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // PerlinNoise2

}
}