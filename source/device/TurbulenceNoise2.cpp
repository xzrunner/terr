#include "terraingraph/device/TurbulenceNoise2.h"
#include "terraingraph/DeviceHelper.h"

#include <heightfield/HeightField.h>

#include <noise/module/turbulence.h>

namespace
{

class NoiseModule : public noise::module::Module
{
public:
    NoiseModule(const hf::HeightField& hf)
        : Module(0), m_hf(hf) {}

    virtual int GetSourceModuleCount() const override { return 0; }

    virtual double GetValue(double x, double y, double z) const override
    {
        if (x < 0 || x > 1 ||
            z < 0 || z > 1) {
            return 0;
        } else {
            return m_hf.Get(
                static_cast<float>(x * m_hf.Width()),
                static_cast<float>(z * m_hf.Height())
            );
        }
    }

private:
    const hf::HeightField& m_hf;

}; // NoiseModule

}

namespace terraingraph
{
namespace device
{

void TurbulenceNoise2::Execute(const std::shared_ptr<dag::Context>& ctx)
{
    auto prev_hf = DeviceHelper::GetInputHeight(*this, 0);
    if (!prev_hf) {
        return;
    }

    auto w = prev_hf->Width();
    auto h = prev_hf->Height();
    m_hf = std::make_shared<hf::HeightField>(w, h);

    noise::module::Turbulence noise;
    noise.SetFrequency(m_frequency);
    noise.SetPower(m_power);
    noise.SetRoughness(m_roughness);
    noise.SetSeed(m_seed);

    NoiseModule prev_mod(*prev_hf);
    noise.SetSourceModule(0, prev_mod);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            const float fx = static_cast<float>(x) / w;
            const float fy = static_cast<float>(y) / h;
            float v = static_cast<float>(noise.GetValue(fx, 0, fy));
            m_hf->Set(x, y, (v + 1.0f) * 0.5f);
        }
    }
}

}
}