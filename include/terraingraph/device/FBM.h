#pragma once

#include "terraingraph/Device.h"

namespace terraingraph
{
namespace device
{

class FBM : public Device
{
public:
    FBM()
    {
        m_exports = {
            {{ DeviceVarType::Heightfield, "out" }},
        };
    }

    virtual void Execute() override;

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "terraingraph/device/FBM.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // FBM

}
}