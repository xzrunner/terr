#pragma once

#include "wm/Device.h"

#include <unirender/Texture.h>

namespace wm
{
namespace device
{

class Combiner : public Device
{
public:
    enum class Method
    {
        Average,
        Add,
        Subtract,
        Multiply,
        Divide,
        Max,
        Min,
    };

public:
    Combiner()
    {
        m_imports = {
            {{ DeviceVarType::Heightfield, "in0" }},
            {{ DeviceVarType::Heightfield, "in1" }},
        };
        m_exports = {
            {{ DeviceVarType::Heightfield, "out" }},
        };
    }

    virtual void Execute() override;

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "wm/device/Combiner.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Combiner

}
}