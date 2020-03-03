#pragma once

#include "terraingraph/Device.h"

namespace terraingraph
{
namespace device
{

class Rock : public Device
{
public:
    Rock()
    {
        m_exports = {
            {{ DeviceVarType::Heightfield, "out" }},
        };
    }

    virtual void Execute(const std::shared_ptr<dag::Context>& ctx = nullptr) override;

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "terraingraph/device/Rock.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Rock

}
}