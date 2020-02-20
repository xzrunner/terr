#pragma once

#include "wm/Device.h"
#include "wm/NoiseQuality.h"

namespace wm
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

#define PARM_FILEPATH "wm/device/PerlinNoise2.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // PerlinNoise2

}
}