#pragma once

#include "terraingraph/Device.h"

namespace terraingraph
{
namespace device
{

class PlasmaFractal : public Device
{
public:
    PlasmaFractal()
    {
        m_exports = {
            {{ DeviceVarType::Heightfield, "out" }},
        };
    }

    virtual void Execute(const std::shared_ptr<dag::Context>& ctx = nullptr) override;

private:
    void MakePlasma(size_t width, size_t height);

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "terraingraph/device/PlasmaFractal.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // PlasmaFractal

}
}