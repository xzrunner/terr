#pragma once

#include "terraingraph/Device.h"

namespace terraingraph
{
namespace device
{

class Chooser : public Device
{
public:
    enum InputID
    {
        ID_A = 0,
        ID_B,
        ID_CTRL,
    };

public:
    Chooser()
    {
        m_imports = {
            {{ DeviceVarType::Any, "a" }},
            {{ DeviceVarType::Any, "b" }},
            {{ DeviceVarType::Heightfield, "ctrl" }},
        };
        m_exports = {
            {{ DeviceVarType::Any, "out" }},
        };
    }

    virtual void Execute(const std::shared_ptr<dag::Context>& ctx = nullptr) override;

private:
    void BlendHeightfield(const hf::HeightField& a,
        const hf::HeightField& b, const hf::HeightField& ctrl);
    void BlendBitmap(const Bitmap& a, const Bitmap& b,
        const hf::HeightField& ctrl);

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "terraingraph/device/Chooser.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Chooser

}
}