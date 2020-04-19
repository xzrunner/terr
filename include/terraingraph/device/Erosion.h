#pragma once

#include "terraingraph/Device.h"

#if defined(_DEBUG)
#define EROSION_GPU
#endif

namespace terraingraph
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

    virtual void Execute(const std::shared_ptr<dag::Context>& ctx = nullptr) override;

private:
#ifdef EROSION_GPU
    void RunGPU(const std::shared_ptr<dag::Context>& ctx);
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

#define PARM_FILEPATH "terraingraph/device/Erosion.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Erosion

}
}