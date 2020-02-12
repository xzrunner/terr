#pragma once

// https://stackoverflow.com/questions/4753055/perlin-noise-generation-for-terrain/4753123#4753123

#include "wm/Device.h"

namespace wm
{
namespace device
{

class BasicNoise : public Device
{
public:
    BasicNoise()
    {
        m_exports = {
            {{ DeviceVarType::Heightfield, "out" }},
        };
    }

    virtual void Execute() override;

private:
    float GetValue(size_t x, size_t y) const;
    float Total(size_t x, size_t y) const;
    float GetValue(float x, float y) const;
    float Interpolate(float x, float y, float a) const;
    float Noise(size_t x, size_t y) const;

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "wm/device/BasicNoise.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // BasicNoise

}
}