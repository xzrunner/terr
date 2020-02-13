#pragma once

#include "wm/Device.h"

namespace ur { class RenderContext; }

namespace wm
{
namespace device
{

class AdvanceNoise : public Device
{
public:
    enum class Type
    {
        Perlin,
        Turbulence,
        BillowyTurbulence,
        RidgedTurbulence,
        IqTurbulence,
        SwissTurbulence,
        JordanTurbulence,
    };

public:
    AdvanceNoise()
    {
        m_exports = {
            {{ DeviceVarType::Heightfield, "out" }},
        };

        Init();
    }

    virtual void Execute() override;

private:
    void Init();

    void InitLookupTextures();
    void InitEval();

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "wm/device/AdvanceNoise.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // AdvanceNoise

}
}