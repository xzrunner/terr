#pragma once

// https://www.scratchapixel.com/lessons/procedural-generation-virtual-worlds/perlin-noise-part-2/perlin-noise-terrain-mesh

#include "terr/Device.h"

#include <unirender/Texture.h>

namespace terr
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
            {{ DeviceVarType::Heightmap, "out" }},
        };
    }

    virtual void Execute(const Context& ctx) override;

private:
    void AddFractalPattern(const terr::PerlinNoise& noise);

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "terr/device/PerlinNoise.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // PerlinNoise

}
}