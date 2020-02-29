#include "terraingraph/device/Lighting.h"
#include "terraingraph/device/AlbedoMap.h"
#include "terraingraph/DeviceHelper.h"
#include "terraingraph/HemanHelper.h"
#include "terraingraph/HeightField.h"
#include "terraingraph/Bitmap.h"

#include <heman.h>

namespace terraingraph
{
namespace device
{

void Lighting::Execute()
{
    auto prev_hf = DeviceHelper::GetInputHeight(*this, 0);
    if (!prev_hf) {
        return;
    }

    auto he_height = HemanHelper::Encode(*prev_hf);
    auto he_albedo = AlbedoMap::Baking(he_height);
    auto he_img = heman_lighting_apply(he_height, he_albedo, 1, 1, 0.5, m_light_pos.xyz);
    auto he_img_data = heman_image_data(he_img);

    size_t w = prev_hf->Width();
    size_t h = prev_hf->Height();

    std::vector<unsigned char> img_data(w * h * 3);
    for (size_t i = 0, n = img_data.size(); i < n; ++i) {
        img_data[i] = static_cast<unsigned char>(he_img_data[i] * 255);
    }

    heman_image_destroy(he_height);
    heman_image_destroy(he_albedo);
    heman_image_destroy(he_img);

    m_bmp = std::make_shared<Bitmap>(w, h);
    m_bmp->SetValues(img_data);
}

}
}