#pragma once

#include "terr/Device.h"

#include <SM_Vector.h>

namespace terr
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
            {{ DeviceVarType::Heightmap, "in" }},
        };
        m_exports = {
            {{ DeviceVarType::Heightmap, "out" }},
        };
    }

    virtual void Execute(const Context& ctx) override;

private:
    float CalcHeight(float h) const;

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "terr/device/Curves.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Curves

}
}