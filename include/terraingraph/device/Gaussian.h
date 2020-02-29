#pragma once

#include "terraingraph/Device.h"

#include <SM_Vector.h>

namespace terraingraph
{
namespace device
{

class Gaussian : public Device
{
public:
    Gaussian()
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

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "terraingraph/device/Gaussian.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Gaussian

}
}