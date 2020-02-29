#pragma once

#include "terraingraph/Device.h"

#include <SM_Vector.h>

namespace terraingraph
{
namespace device
{

class NormalMap : public Device
{
public:
    NormalMap()
    {
        m_imports = {
            {{ DeviceVarType::Heightfield, "in" }},
        };
        m_exports = {
            {{ DeviceVarType::Bitmap, "out" }},
        };
    }

    virtual void Execute() override;

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "terraingraph/device/NormalMap.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // NormalMap

}
}