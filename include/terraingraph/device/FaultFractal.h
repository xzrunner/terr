#pragma once

#include "terraingraph/Device.h"

namespace terraingraph
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

    virtual void Execute(const std::shared_ptr<dag::Context>& ctx = nullptr) override;

private:
    void MakeFault(size_t width, size_t height);

    static void FilterHeightBand(float* band, int stride, int count, float filter);
    static void FilterHeightField(size_t width, size_t height, std::vector<float>& height_data, float filter);

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "terraingraph/device/FaultFractal.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // FaultFractal

}
}