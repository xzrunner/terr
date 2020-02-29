#pragma once

#include "terraingraph/Device.h"

#include <SM_Vector.h>

namespace terraingraph
{
namespace device
{

class Gradient : public Device
{
public:
    Gradient()
    {
        m_exports = {
            {{ DeviceVarType::Heightfield, "out" }},
        };
    }

    virtual void Execute() override;

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "terraingraph/device/Gradient.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Gradient

}
}