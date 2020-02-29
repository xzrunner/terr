#pragma once

#include "terraingraph/Device.h"

#include <SM_Vector.h>

namespace terraingraph
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

    virtual void Execute() override;

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "terraingraph/device/OverlayView.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // OverlayView

}
}