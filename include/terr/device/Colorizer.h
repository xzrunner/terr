#pragma once

#include "terr/Device.h"

#include <SM_Vector.h>

namespace terr
{
namespace device
{

class Colorizer : public Device
{
public:
    Colorizer()
    {
        m_imports = {
            {{ DeviceVarType::Heightfield, "in" }},
        };
        m_exports = {
            {{ DeviceVarType::Bitmap, "out" }},
        };
    }

    virtual void Execute(const Context& ctx) override;

private:
    void SortGradientColors();

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "terr/device/Colorizer.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Colorizer

}
}