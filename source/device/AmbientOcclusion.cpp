#include "terraingraph/device/AmbientOcclusion.h"
#include "terraingraph/DeviceHelper.h"
#include "terraingraph/HemanHelper.h"

#include <heightfield/HeightField.h>

#include <heman.h>

namespace terraingraph
{
namespace device
{

void AmbientOcclusion::Execute(const std::shared_ptr<dag::Context>& ctx)
{
    auto prev_hf = DeviceHelper::GetInputHeight(*this, 0);
    if (!prev_hf) {
        return;
    }

    size_t w = prev_hf->Width();
    size_t h = prev_hf->Height();

    auto he_height = HemanHelper::Encode(*prev_hf);

    heman_lighting_set_occlusion_scale(m_scale);
    auto he_ao = heman_lighting_compute_occlusion(he_height);
    auto he_ao_data = heman_image_data(he_ao);

    m_bmp = std::make_shared<Bitmap>(w, h);

    std::vector<unsigned char> ao_data(w * h * 3);
    for (size_t i = 0, n = w * h; i < n; ++i) {
        const auto ao = static_cast<unsigned char>(he_ao_data[i] * 255);
        for (size_t j = 0; j < 3; ++j) {
            ao_data[i * 3 + j] = ao;
        }
    }

    heman_image_destroy(he_height);
    heman_image_destroy(he_ao);

    m_bmp->SetValues(ao_data);
}

}
}