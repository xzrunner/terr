#pragma once

#include "terr/Device.h"

#include <unirender/Texture.h>

namespace terr
{
namespace device
{

class Erosion : public Device
{
public:
    Erosion()
    {
        m_imports = {
            {{ DeviceVarType::Any, "in" }},
        };
        m_exports = {
            {{ DeviceVarType::Any, "out" }},
        };
    }

    virtual void Execute(const Context& ctx) override;

private:
    void InitializeBrushIndices(size_t size);

private:
    size_t m_map_sz = 0;

    std::vector<std::vector<size_t>> m_brush_indices;
    std::vector<std::vector<float>>  m_brush_weights;

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "terr/device/Erosion.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Erosion

}
}