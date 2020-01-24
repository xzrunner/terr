#pragma once

#include "terr/Device.h"

#include <unirender/Texture.h>

namespace terr
{
namespace device
{

class PlasmaFractal : public Device
{
public:
    PlasmaFractal()
    {
        m_exports = {
            {{ DeviceVarType::Any, "out" }},
        };
    }

    virtual void Execute(const Context& ctx) override;

private:
    void MakePlasma(size_t size);

private:
    // Scale the terrain height values to a range of 0-255
    static void NormalizeTerrain(std::vector<float>& height_data);

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "terr/device/PlasmaFractal.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // PlasmaFractal

}
}