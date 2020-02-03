#include "terr/device/Chooser.h"
#include "terr/DeviceHelper.h"
#include "terr/HeightField.h"
#include "terr/Bitmap.h"

namespace terr
{
namespace device
{

void Chooser::Execute(const Context& ctx)
{
    auto blend = DeviceHelper::GetInputHeight(*this, ID_CTRL);
    if (!blend) {
        return;
    }

    m_hf.reset();
    m_bmp.reset();

    auto hf0 = DeviceHelper::GetInputHeight(*this, ID_A);
    auto hf1 = DeviceHelper::GetInputHeight(*this, ID_B);
    if (hf0 && hf1)
    {
        BlendHeightfield(*hf0, *hf1, *blend);
    }
    else
    {
        auto bmp0 = DeviceHelper::GetInputBitmap(*this, ID_A);
        auto bmp1 = DeviceHelper::GetInputBitmap(*this, ID_B);
        if (bmp0 && bmp1) {
            BlendBitmap(*bmp0, *bmp1, *blend);
        }
    }
}

void Chooser::BlendHeightfield(const HeightField& a,
                               const HeightField& b,
                               const HeightField& ctrl)
{
    if (a.Width() == 0 || a.Height() == 0 ||
        a.Width() != b.Width() || a.Height() != b.Height() ||
        a.Width() != ctrl.Width() || a.Height() != ctrl.Height()) {
        return;
    }

    auto& a_vals = a.GetValues();
    auto& b_vals = b.GetValues();
    auto& ctrl_vals = ctrl.GetValues();
    assert(a_vals.size() == b_vals.size()
        || a_vals.size() == ctrl_vals.size());

    std::vector<float> vals(a_vals.size(), 0);
    for (size_t i = 0, n = a_vals.size(); i < n; ++i) {
        vals[i] = a_vals[i] + (b_vals[i] - a_vals[i]) * ctrl_vals[i];
    }
    m_hf = std::make_shared<HeightField>(a.Width(), a.Height());
    m_hf->SetValues(vals);
}

void Chooser::BlendBitmap(const Bitmap& a, const Bitmap& b,
                          const HeightField& ctrl)
{
    if (a.Width() == 0 || a.Height() == 0 ||
        a.Width() != b.Width() || a.Height() != b.Height() ||
        a.Width() != ctrl.Width() || a.Height() != ctrl.Height()) {
        return;
    }

    auto& a_vals = a.GetValues();
    auto& b_vals = b.GetValues();
    auto& ctrl_vals = ctrl.GetValues();
    assert(a_vals.size() == b_vals.size()
        || a_vals.size() == ctrl_vals.size());

    std::vector<unsigned char> vals(a_vals.size(), 0);
    for (size_t i = 0, n = ctrl_vals.size(); i < n; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            auto d = (b_vals[i * 3 + j] - a_vals[i * 3 + j]) * ctrl_vals[i];
            vals[i * 3 + j] = a_vals[i * 3 + j] + static_cast<unsigned char>(d);
        }
    }
    m_bmp = std::make_shared<Bitmap>(a.Width(), a.Height());
    m_bmp->SetValues(vals);
}

}
}