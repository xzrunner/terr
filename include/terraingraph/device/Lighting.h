#pragma once

#include "terraingraph/Device.h"

#include <SM_Vector.h>

namespace terraingraph
{
namespace device
{

class Lighting : public Device
{
public:
    Lighting()
    {
        m_imports = {
            {{ DeviceVarType::Heightfield, "heightmap" }},
            {{ DeviceVarType::Bitmap,      "albedo" }},
        };
        m_exports = {
            {{ DeviceVarType::Bitmap, "out" }},
        };
    }

    virtual void Execute() override;

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "terraingraph/device/Lighting.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Lighting

}
}