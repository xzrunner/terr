#pragma once

#include "wm/Device.h"

#include <unirender/Texture.h>

namespace wm
{
namespace device
{

class FaultFractal : public Device
{
public:
    FaultFractal()
    {
        m_exports = {
            {{ DeviceVarType::Heightfield, "out" }},
        };
    }

    virtual void Execute(const Context& ctx) override;

private:
    void MakeFault(size_t size);

    static void FilterHeightBand(float* band, int stride, int count, float filter);
    static void FilterHeightField(int size, std::vector<float>& height_data, float filter);

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "wm/device/FaultFractal.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // FaultFractal

}
}