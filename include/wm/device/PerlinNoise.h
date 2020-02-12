#pragma once

// https://www.scratchapixel.com/lessons/procedural-generation-virtual-worlds/perlin-noise-part-2/perlin-noise-terrain-mesh

#include "wm/Device.h"

#include <unirender/Texture.h>

namespace wm
{

class PerlinNoise;

namespace device
{

class PerlinNoise : public Device
{
public:
    PerlinNoise()
    {
        m_exports = {
            {{ DeviceVarType::Heightfield, "out" }},
        };
    }

    virtual void Execute() override;

private:
    void AddFractalPattern(const wm::PerlinNoise& noise);

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "wm/device/PerlinNoise.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // PerlinNoise

}
}