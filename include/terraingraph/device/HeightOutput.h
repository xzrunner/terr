#pragma once

#include "terraingraph/Device.h"

#include <SM_Vector.h>

namespace terraingraph
{
namespace device
{

class HeightOutput : public Device
{
public:
    HeightOutput()
    {
        m_imports = {
            {{ DeviceVarType::Heightfield, "in" }},
        };
    }

    virtual void Execute() override;

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "terraingraph/device/HeightOutput.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // HeightOutput

}
}