#pragma once

#include "terraingraph/Device.h"

namespace ur { class Device; }

namespace terraingraph
{
namespace device
{

class Resample : public Device
{
public:
    Resample()
    {
        m_imports = {
            {{ DeviceVarType::Any, "in" }},
        };
        m_exports = {
            {{ DeviceVarType::Any, "out" }},
        };
    }

    virtual void Execute(const std::shared_ptr<dag::Context>& ctx = nullptr) override;

    static std::shared_ptr<hf::HeightField>
        ResampleHeightField(const ur::Device& dev, const hf::HeightField& hf, int width, int height);

private:
    void ResampleMask(const Mask& mask);

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "terraingraph/device/Resample.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Resample

}
}