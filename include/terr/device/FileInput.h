#pragma once

#include "terr/Device.h"

#include <unirender/Texture.h>

namespace terr
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

    virtual void Execute(const Context& ctx) override;

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "terr/device/FileInput.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // FileInput

}
}