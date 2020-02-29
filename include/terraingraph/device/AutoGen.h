#pragma once

#include "terraingraph/Device.h"

namespace terraingraph
{
namespace device
{

class AutoGen : public Device
{
public:
    AutoGen()
    {
        m_imports = {
            {{ DeviceVarType::Mask, "mask" }},
        };
        m_exports = {
            {{ DeviceVarType::Heightfield, "out" }},
        };
    }

    virtual void Execute() override;

private:
    std::shared_ptr<HeightField>
        MaskToHeightField(const Mask& mask) const;

    void InitDistMap(const Mask& mask, std::vector<float>& dist_map) const;
    void SplitDistMap(const Mask& mask, std::vector<float>& dist_map) const;
    void BuildDistMap(const Mask& mask, std::vector<float>& dist_map) const;

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "terraingraph/device/AutoGen.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // AutoGen

}
}