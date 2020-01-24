#pragma once

// https://stackoverflow.com/questions/4753055/perlin-noise-generation-for-terrain/4753123#4753123

#include "terr/Device.h"

namespace terr
{
namespace device
{

class BasicNoise : public Device
{
public:
    BasicNoise()
    {
        m_exports = {
            {{ DeviceVarType::Any, "out" }},
        };
    }

    virtual void Execute(const Context& ctx) override;

private:
    float GetHeight(size_t x, size_t y) const;
    float Total(size_t x, size_t y) const;
    float GetValue(float x, float y) const;
    float Interpolate(float x, float y, float a) const;
    float Noise(size_t x, size_t y) const;

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "terr/device/BasicNoise.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // BasicNoise

}
}