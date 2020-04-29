#include "terraingraph/device/Chooser.h"
#include "terraingraph/DeviceHelper.h"
#include "terraingraph/Context.h"

#include <heightfield/HeightField.h>
#include <heightfield/Utility.h>

namespace terraingraph
{
namespace device
{

void Chooser::Execute(const std::shared_ptr<dag::Context>& ctx)
{
    auto blend = DeviceHelper::GetInputHeight(*this, ID_CTRL);
    if (!blend) {
        return;
    }

    m_hf.reset();
    m_bmp.reset();

    auto& dev = *std::static_pointer_cast<Context>(ctx)->ur_dev;

    auto hf0 = DeviceHelper::GetInputHeight(*this, ID_A);
    auto hf1 = DeviceHelper::GetInputHeight(*this, ID_B);
    if (hf0 && hf1)
    {
        BlendHeightfield(dev, *hf0, *hf1, *blend);
    }
    else
    {
        auto bmp0 = DeviceHelper::GetInputBitmap(*this, ID_A);
        auto bmp1 = DeviceHelper::GetInputBitmap(*this, ID_B);
        if (bmp0 && bmp1) {
            BlendBitmap(dev, *bmp0, *bmp1, *blend);
        }
    }
}

void Chooser::BlendHeightfield(const ur::Device& dev,
                               const hf::HeightField& a,
                               const hf::HeightField& b,
                               const hf::HeightField& ctrl)
{
    if (a.Width() == 0 || a.Height() == 0 ||
        a.Width() != b.Width() || a.Height() != b.Height() ||
        a.Width() != ctrl.Width() || a.Height() != ctrl.Height()) {
        return;
    }

    auto& a_vals = a.GetValues(dev);
    auto& b_vals = b.GetValues(dev);
    auto& ctrl_vals = ctrl.GetValues(dev);
    assert(a_vals.size() == b_vals.size()
        || a_vals.size() == ctrl_vals.size());

    std::vector<int32_t> vals(a_vals.size(), 0);
    for (size_t i = 0, n = a_vals.size(); i < n; ++i) {
        vals[i] = a_vals[i] + static_cast<int32_t>((b_vals[i] - a_vals[i]) *
            hf::Utility::HeightShortToDouble(ctrl_vals[i]));
    }
    m_hf = std::make_shared<hf::HeightField>(a.Width(), a.Height());
    m_hf->SetValues(vals);
}

void Chooser::BlendBitmap(const ur::Device& dev, const Bitmap& a,
                          const Bitmap& b, const hf::HeightField& ctrl)
{
    if (a.Width() == 0 || a.Height() == 0 ||
        a.Width() != b.Width() || a.Height() != b.Height() ||
        a.Width() != ctrl.Width() || a.Height() != ctrl.Height()) {
        return;
    }

    auto a_vals = a.GetPixels();
    auto b_vals = b.GetPixels();
    auto& ctrl_vals = ctrl.GetValues(dev);

    m_bmp = std::make_shared<Bitmap>(a.Width(), a.Height());
    auto dst = m_bmp->GetPixels();

    std::vector<unsigned char> vals(a.Width() * a.Height() * a.Channels(), 0);
    for (size_t i = 0, n = ctrl_vals.size(); i < n; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            auto d = (b_vals[i * 3 + j] - a_vals[i * 3 + j]) * ctrl_vals[i];
            dst[i * 3 + j] = a_vals[i * 3 + j] + static_cast<unsigned char>(d);
        }
    }
}

}
}