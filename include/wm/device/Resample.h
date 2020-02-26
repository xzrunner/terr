#pragma once

#include "wm/Device.h"

namespace wm
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

    virtual void Execute() override;

private:
    void ResampleMask(const Mask& mask);
    void ResampleHeightField(const HeightField& hf);

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "wm/device/Resample.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Resample

}
}