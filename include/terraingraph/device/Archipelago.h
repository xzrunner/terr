#pragma once

#include "terraingraph/Device.h"

#include <SM_Vector.h>

namespace terraingraph
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

#define PARM_FILEPATH "terraingraph/device/Archipelago.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Archipelago

}
}