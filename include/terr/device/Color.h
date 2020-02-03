#pragma once

#include "terr/Device.h"

#include <unirender/Texture.h>
#include <SM_Vector.h>

namespace terr
{
namespace device
{

class Color : public Device
{
public:
    Color()
    {
        m_exports = {
            {{ DeviceVarType::Bitmap, "out" }},
        };
    }

    virtual void Execute(const Context& ctx) override;

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "terr/device/Color.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Color

}
}