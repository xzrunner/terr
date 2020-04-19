#pragma once

#include "terraingraph/Device.h"
#include "terraingraph/NoiseQuality.h"

#include <SM_Matrix.h>

namespace terraingraph
{
namespace device
{

class TemplateBrush : public Device
{
public:
    TemplateBrush()
    {
        m_imports = {
            {{ DeviceVarType::Heightfield, "in" }},
        };
        m_exports = {
            {{ DeviceVarType::Heightfield, "out" }},
        };
    }

    virtual void Execute(const std::shared_ptr<dag::Context>& ctx = nullptr) override;

    auto GetBrush() const { return m_brush; }

private:
    void InitEval(const std::shared_ptr<dag::Context>& ctx);

private:
    std::shared_ptr<hf::HeightField> m_brush = nullptr;

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "terraingraph/device/TemplateBrush.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // TemplateBrush

}
}