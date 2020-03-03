#pragma once

#include "terraingraph/Device.h"

#define THERMAL_WEATHERING_GPU

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

        Init();
    }

    virtual void Execute(const std::shared_ptr<dag::Context>& ctx = nullptr) override;

private:
    void Init();

    void StepCPU();
    void StepGPU(int thread_group_count);

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "terraingraph/device/ThermalWeathering.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // ThermalWeathering

}
}