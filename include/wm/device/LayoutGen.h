#pragma once

#include "wm/Device.h"

#include <unirender/Texture.h>
#include <SM_Vector.h>

namespace wm
{
namespace device
{

class LayoutGen : public Device
{
public:
    LayoutGen()
    {
        m_imports = {
            {{ DeviceVarType::Mask, "mask" }},
        };
        m_exports = {
            {{ DeviceVarType::Heightfield, "out" }},
        };
    }

    virtual void Execute(const Context& ctx) override;

private:
    std::shared_ptr<HeightField>
        MaskToHeightField(const Mask& mask) const;

    static std::vector<float>
        CalcMaskDistMap(const Mask& mask);

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "wm/device/LayoutGen.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // LayoutGen

}
}