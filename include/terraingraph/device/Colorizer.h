#pragma once

#include "terraingraph/Device.h"

#include <SM_Vector.h>

namespace terraingraph
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

    virtual void Execute(const std::shared_ptr<dag::Context>& ctx = nullptr) override;

private:
    void SortGradientColors();

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "terraingraph/device/Colorizer.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Colorizer

}
}