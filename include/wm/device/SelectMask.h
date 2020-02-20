#pragma once

#include "wm/Device.h"

namespace wm
{
namespace device
{

class SelectMask : public Device
{
public:
    enum InputID
    {
        ID_SOURCE = 0,
        ID_MASK
    };

public:
    SelectMask()
    {
        m_imports = {
            {{ DeviceVarType::Heightfield, "in" }},
            {{ DeviceVarType::Mask,        "mask" }},
        };
        m_exports = {
            {{ DeviceVarType::Heightfield, "out" }},
        };
    }

    virtual void Execute() override;

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "wm/device/SelectMask.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // SelectMask

}
}