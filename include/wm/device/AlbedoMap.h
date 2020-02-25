#pragma once

#include "wm/Device.h"

#include <SM_Vector.h>

#include <heman.h>

namespace wm
{

class HeightField;

namespace device
{

class AlbedoMap : public Device
{
public:
    AlbedoMap()
    {
        m_imports = {
            {{ DeviceVarType::Heightfield, "heightmap" }},
            {{ DeviceVarType::Bitmap,      "gradient" }},
        };
        m_exports = {
            {{ DeviceVarType::Bitmap, "out" }},
        };
    }

    virtual void Execute() override;

    static heman_image* Baking(heman_image* height, float min_height = 0.0f,
        float max_height = 1.0f, bool contour_lines = true);

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "wm/device/AlbedoMap.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // AlbedoMap

}
}