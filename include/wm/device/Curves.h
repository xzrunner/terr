#pragma once

#include "wm/Device.h"

#include <SM_Vector.h>

namespace wm
{
namespace device
{

class Curves : public Device
{
public:
    enum class Type
    {
        Linear,
        Spline,
    };

public:
    Curves()
    {
        m_imports = {
            {{ DeviceVarType::Heightfield, "in" }},
        };
        m_exports = {
            {{ DeviceVarType::Heightfield, "out" }},
        };
    }

    virtual void Execute(const Context& ctx) override;

private:
    float CalcHeight(float h) const;

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "wm/device/Curves.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Curves

}
}