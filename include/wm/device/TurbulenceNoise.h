#pragma once

#include "wm/Device.h"

#include <SM_Matrix.h>

namespace ur { class RenderContext; }

namespace wm
{
namespace device
{

class TurbulenceNoise : public Device
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
        Erosion1Turbulence,
        Erosion2Turbulence,
    };

public:
    TurbulenceNoise()
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

    void CalcOctaveAmps(sm::mat4& noise_octave_amps,
        sm::mat4& distort_octave_amps);

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "wm/device/TurbulenceNoise.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // TurbulenceNoise

}
}