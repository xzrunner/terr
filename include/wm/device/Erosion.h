#pragma once

#include "wm/Device.h"

#define EROSION_GPU

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

        Init();
    }

    virtual void Execute() override;

private:
    void Init();

#ifdef EROSION_GPU
    void RunGPU();
#else
    void RunCPU();
#endif // EROSION_GPU

#ifndef EROSION_GPU
    void InitializeBrushIndices(size_t width, size_t height);
#endif // EROSION_GPU

private:
#ifndef EROSION_GPU
    std::vector<std::vector<size_t>> m_brush_indices;
    std::vector<std::vector<float>>  m_brush_weights;
#endif // EROSION_GPU

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "wm/device/Erosion.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Erosion

}
}