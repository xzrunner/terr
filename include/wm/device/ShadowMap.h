#pragma once

#include "wm/Device.h"

#include <SM_Vector.h>

namespace wm
{
namespace device
{

class ShadowMap : public Device
{
public:
    ShadowMap()
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

#define PARM_FILEPATH "wm/device/ShadowMap.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // ShadowMap

}
}