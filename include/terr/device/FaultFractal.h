#pragma once

#include "terr/Device.h"

#include <unirender/Texture.h>

namespace terr
{
namespace device
{

class FaultFractal : public Device
{
public:
    FaultFractal()
    {
        m_exports = {
            {{ DeviceVarType::Any, "out" }},
        };
    }

    virtual void Execute(const Context& ctx) override;

private:
    void MakeFault(size_t size);

    static void FilterHeightBand(float* band, int stride, int count, float filter);
    static void FilterHeightField(int size, std::vector<float>& height_data, float filter);

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "terr/device/FaultFractal.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // FaultFractal

}
}