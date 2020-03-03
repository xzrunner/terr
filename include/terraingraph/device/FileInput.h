#pragma once

#include "terraingraph/Device.h"

namespace terraingraph
{
namespace device
{

class FileInput : public Device
{
public:
    FileInput()
    {
        m_exports = {
            {{ DeviceVarType::Heightfield, "heightfield" }},
            {{ DeviceVarType::Mask,        "mask" }},
        };
    }

    virtual void Execute(const std::shared_ptr<dag::Context>& ctx = nullptr) override;

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "terraingraph/device/FileInput.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // FileInput

}
}