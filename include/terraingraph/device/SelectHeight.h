#pragma once

#include "terraingraph/Device.h"

namespace terraingraph
{
namespace device
{

class SelectHeight : public Device
{
public:
    SelectHeight()
    {
        m_imports = {
            {{ DeviceVarType::Heightfield, "in" }},
        };
        m_exports = {
            {{ DeviceVarType::Heightfield, "out" }},
        };
    }

    virtual void Execute() override;

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "terraingraph/device/SelectHeight.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // SelectHeight

}
}