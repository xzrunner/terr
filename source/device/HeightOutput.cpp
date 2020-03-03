#include "terraingraph/device/HeightOutput.h"
#include "terraingraph/DeviceHelper.h"

#include <heightfield/HeightField.h>
#include <gimg_export.h>
#include <gimg_typedef.h>

#define OUTPUT_SINGLE_CHANNEL

namespace terraingraph
{
namespace device
{

void HeightOutput::Execute(const std::shared_ptr<dag::Context>& ctx)
{
    auto prev_hf = DeviceHelper::GetInputHeight(*this, 0);
    if (!prev_hf || m_filepath.empty()) {
        return;
    }

    auto w = prev_hf->Width();
    auto h = prev_hf->Height();

    auto& vals = prev_hf->GetValues();
    assert(vals.size() == w * h);
#ifdef OUTPUT_SINGLE_CHANNEL
    std::vector<uint8_t> pixels(vals.size());
    for (size_t i = 0, n = vals.size(); i < n; ++i) {
        pixels[i] = static_cast<uint8_t>(vals[i] * 255.0f);
    }
    gimg_export(m_filepath.c_str(), pixels.data(), w, h, GPF_LUMINANCE, 1);
#else
    std::vector<uint8_t> pixels(vals.size() * 3);
    for (size_t i = 0, n = vals.size(); i < n; ++i) {
        auto v = static_cast<uint8_t>(vals[i] * 255.0f);
        for (size_t j = 0; j < 3; ++j) {
            pixels[i * 3 + j] = v;
        }
    }
    gimg_export(m_filepath.c_str(), pixels.data(), w, h, GPF_RGB, 1);
#endif // OUTPUT_SINGLE_CHANNEL
}

}
}