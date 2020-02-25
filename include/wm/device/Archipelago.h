#pragma once

#include "wm/Device.h"

#include <SM_Vector.h>

namespace wm
{
namespace device
{

class Archipelago : public Device
{
public:
    Archipelago()
    {
        m_exports = {
            {{ DeviceVarType::Heightfield, "out" }},
        };
    }

    virtual void Execute() override;

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "wm/device/Archipelago.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Archipelago

}
}