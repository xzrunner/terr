#pragma once

#include "wm/Device.h"

#include <SM_Vector.h>

namespace wm
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

    virtual void Execute() override;

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "wm/device/Color.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Color

}
}