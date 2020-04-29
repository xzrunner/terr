#include "terraingraph/device/TurbulenceNoise2.h"
#include "terraingraph/DeviceHelper.h"
#include "terraingraph/Context.h"

#include <heightfield/HeightField.h>
#include <heightfield/Utility.h>

#include <noise/module/turbulence.h>

namespace
{

class NoiseModule : public noise::module::Module
{
public:
    NoiseModule(const ur::Device& dev, const hf::HeightField& hf)
        : Module(0)
        , m_dev(dev)
        , m_hf(hf)
    {
    }

    virtual int GetSourceModuleCount() const override { return 0; }

    virtual double GetValue(double x, double y, double z) const override
    {
        if (x < 0 || x > 1 ||
            z < 0 || z > 1) {
            return 0;
        } else {
            return m_hf.Get(m_dev,
                static_cast<float>(x * m_hf.Width()),
                static_cast<float>(z * m_hf.Height())
            );
        }
    }

private:
    const ur::Device& m_dev;

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

    auto& dev = *std::static_pointer_cast<Context>(ctx)->ur_dev;
    NoiseModule prev_mod(dev, *prev_hf);
    noise.SetSourceModule(0, prev_mod);

    for (size_t y = 0; y < h; ++y) {
        for (size_t x = 0; x < w; ++x) {
            const float fx = static_cast<float>(x) / w;
            const float fy = static_cast<float>(y) / h;
            double v = noise.GetValue(fx, 0, fy);
            m_hf->Set(x, y, hf::Utility::HeightDoubleToShort(v));
        }
    }
}

}
}