#pragma once

#include "wm/Device.h"

#include <SM_Vector.h>

namespace wm
{
namespace device
{

class OverlayView : public Device
{
public:
    enum InputID
    {
        ID_HEIGHTFIELD = 0,
        ID_BITMAP
    };

public:
    OverlayView()
    {
        m_imports = {
            {{ DeviceVarType::Heightfield, "height" }},
            {{ DeviceVarType::Bitmap,      "texture" }},
        };
    }

    virtual void Execute(const Context& ctx) override;

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "wm/device/OverlayView.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // OverlayView

}
}