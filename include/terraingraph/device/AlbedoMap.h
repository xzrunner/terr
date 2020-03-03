#pragma once

#include "terraingraph/Device.h"

#include <SM_Vector.h>

#include <heman.h>

namespace hf { class HeightField; }

namespace terraingraph
{
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

    virtual void Execute(const std::shared_ptr<dag::Context>& ctx = nullptr) override;

    static heman_image* Baking(heman_image* height, float min_height = 0.0f,
        float max_height = 1.0f, bool contour_lines = false);

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "terraingraph/device/AlbedoMap.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // AlbedoMap

}
}