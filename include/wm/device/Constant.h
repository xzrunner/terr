#pragma once

#include "wm/Device.h"

#include <unirender/Texture.h>
#include <SM_Vector.h>

namespace wm
{
namespace device
{

class Constant : public Device
{
public:
    Constant()
    {
        m_exports = {
            {{ DeviceVarType::Heightfield, "out" }},
        };
    }

    virtual void Execute() override;

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "wm/device/Constant.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Constant

}
}