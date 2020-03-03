#include "terraingraph/device/NormalMap.h"
#include "terraingraph/DeviceHelper.h"
#include "terraingraph/HemanHelper.h"
#include "terraingraph/Bitmap.h"

#include <heightfield/HeightField.h>

#include <heman.h>

namespace terraingraph
{
namespace device
{

void NormalMap::Execute(const std::shared_ptr<dag::Context>& ctx)
{
    auto prev_hf = DeviceHelper::GetInputHeight(*this, 0);
    if (!prev_hf) {
        return;
    }

    size_t w = prev_hf->Width();
    size_t h = prev_hf->Height();

    auto he_height = HemanHelper::Encode(*prev_hf);

    auto he_norm = heman_lighting_compute_normals(he_height);
    auto he_norm_data = heman_image_data(he_norm);

    m_bmp = std::make_shared<Bitmap>(w, h);

    std::vector<unsigned char> norm_data(w * h * 3);
    for (size_t i = 0, n = norm_data.size(); i < n; ++i) {
        const float flt01 = (he_norm_data[i] + 1) * 0.5f;
        norm_data[i] = static_cast<unsigned char>(flt01 * 255);
    }

    heman_image_destroy(he_height);
    heman_image_destroy(he_norm);

    m_bmp->SetValues(norm_data);
}

}
}