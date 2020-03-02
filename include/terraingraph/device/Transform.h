#pragma once

#include "terraingraph/Device.h"
#include "terraingraph/NoiseQuality.h"

#include <SM_Vector.h>

namespace terraingraph
{
namespace device
{

class Transform : public Device
{
public:
    Transform()
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

#define PARM_FILEPATH "terraingraph/device/Transform.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Transform

}
}