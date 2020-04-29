#pragma once

#include "terraingraph/Device.h"

#define THERMAL_WEATHERING_GPU

namespace ur { class Device; }

namespace terraingraph
{
namespace device
{

class ThermalWeathering : public Device
{
public:
    ThermalWeathering()
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
    void StepCPU(const ur::Device& dev);
    void StepGPU(const ur::Device& dev, int thread_group_count);

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "terraingraph/device/ThermalWeathering.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // ThermalWeathering

}
}