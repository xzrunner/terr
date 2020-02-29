#pragma once

#include "terraingraph/Device.h"

#include <SM_Vector.h>

namespace terraingraph
{
namespace device
{

class CellularNoise : public Device
{
public:
    CellularNoise()
    {
        m_exports = {
            {{ DeviceVarType::Heightfield, "out" }},
        };

        Init();
    }

    virtual void Execute() override;

private:
    void Init();

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "terraingraph/device/CellularNoise.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // CellularNoise

}
}