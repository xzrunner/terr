#pragma once

#include "wm/Device.h"

#include <SM_Vector.h>

namespace wm
{
namespace device
{

class Wind : public Device
{
public:
    Wind()
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

#define PARM_FILEPATH "wm/device/Wind.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Wind

}
}