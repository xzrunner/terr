#pragma once

// https://www.scratchapixel.com/lessons/procedural-generation-virtual-worlds/perlin-noise-part-2/perlin-noise-terrain-mesh

#include "terraingraph/Device.h"

namespace terraingraph
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
    void AddFractalPattern(const terraingraph::PerlinNoise& noise);

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "terraingraph/device/PerlinNoise.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // PerlinNoise

}
}