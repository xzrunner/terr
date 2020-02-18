#include "wm/device/HeightOutput.h"
#include "wm/DeviceHelper.h"
#include "wm/HeightField.h"

#include <gimg_export.h>
#include <gimg_typedef.h>

namespace wm
{
namespace device
{

void HeightOutput::Execute()
{
    auto prev_hf = DeviceHelper::GetInputHeight(*this, 0);
    if (!prev_hf || m_filepath.empty()) {
        return;
    }

    auto w = prev_hf->Width();
    auto h = prev_hf->Height();

    auto& vals = prev_hf->GetValues();
    assert(vals.size() == w * h);
    std::vector<uint8_t> pixels(vals.size());
    for (size_t i = 0, n = vals.size(); i < n; ++i) {
        pixels[i] = static_cast<uint8_t>(vals[i] * 255.0f);
    }
    gimg_export(m_filepath.c_str(), pixels.data(), w, h, GPF_LUMINANCE, 1);
}

}
}