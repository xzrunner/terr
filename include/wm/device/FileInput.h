#pragma once

#include "wm/Device.h"

namespace wm
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

    virtual void Execute() override;

    static std::shared_ptr<HeightField> LoadHeightField(const std::string& filepath);

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "wm/device/FileInput.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // FileInput

}
}