#pragma once

#include "terraingraph/Device.h"

#include <SM_Vector.h>

#define NORMAL_MAP_GPU

namespace terraingraph
{
namespace device
{

class NormalMap : public Device
{
public:
    NormalMap()
    {
        m_imports = {
            {{ DeviceVarType::Heightfield, "in" }},
        };
        m_exports = {
            {{ DeviceVarType::Bitmap, "out" }},
        };

#ifdef NORMAL_MAP_GPU
        Init();
#endif // NORMAL_MAP_GPU
    }

    virtual void Execute(const std::shared_ptr<dag::Context>& ctx = nullptr) override;

private:
    void Init();

    void RunGPU(const hf::HeightField& hf);
    void RunCPU(const hf::HeightField& hf);

    RTTR_ENABLE(Device)

#define PARM_FILEPATH "terraingraph/device/NormalMap.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // NormalMap

}
}