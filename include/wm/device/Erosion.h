#pragma once

#include "wm/Device.h"

namespace wm
{
namespace device
{

class Erosion : public Device
{
public:
    Erosion()
    {
        m_imports = {
            {{ DeviceVarType::Heightfield, "in" }},
        };
        m_exports = {
            {{ DeviceVarType::Heightfield, "out" }},
        };
    }

    virtual void Execute() override;

private:
    void InitializeBrushIndices(size_t width, size_t height);

private:
    std::vector<std::vector<size_t>> m_brush_indices;
    std::vector<std::vector<float>>  m_brush_weights;

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "wm/device/Erosion.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Erosion

}
}