#pragma once

#include "wm/Device.h"

#include <unirender/Texture.h>

namespace wm
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

    virtual void Execute() override;

private:
    void BlendHeightfield(const HeightField& a,
        const HeightField& b, const HeightField& ctrl);
    void BlendBitmap(const Bitmap& a, const Bitmap& b,
        const HeightField& ctrl);

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "wm/device/Chooser.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Chooser

}
}