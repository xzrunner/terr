#include "wm/device/AlbedoMap.h"
#include "wm/DeviceHelper.h"
#include "wm/HemanHelper.h"
#include "wm/HeightField.h"
#include "wm/Bitmap.h"

namespace
{

#define COUNT(a) (sizeof(a) / sizeof(a[0]))

// Create a reasonable ocean-to-land color gradient.
int cp_locations[] = {
    000,  // Dark Blue
    126,  // Light Blue
    127,  // Yellow
    128,  // Dark Green
    160,  // Brown
    200,  // White
    255,  // White
};

heman_color cp_colors[] = {
    0x001070,  // Dark Blue
    0x2C5A7C,  // Light Blue
    0xE0F0A0,  // Yellow
    0x5D943C,  // Dark Green
    0x606011,  // Brown
    0xFFFFFF,  // White
    0xFFFFFF,  // White
};

}

namespace wm
{
namespace device
{

void AlbedoMap::Execute()
{
    auto prev_hf = DeviceHelper::GetInputHeight(*this, 0);
    if (!prev_hf) {
        return;
    }

    auto he_height = HemanHelper::Encode(*prev_hf);
    auto he_albedo = Baking(he_height, m_min_height, m_max_height);
    auto he_albedo_data = heman_image_data(he_albedo);

    size_t w = prev_hf->Width();
    size_t h = prev_hf->Height();
    m_bmp = std::make_shared<Bitmap>(w, h);

    // filling
    std::vector<unsigned char> albedo_data(w * h * 3);
    for (size_t i = 0, n = albedo_data.size(); i < n; ++i) {
        albedo_data[i] = static_cast<unsigned char>(he_albedo_data[i] * 255);
    }

    heman_image_destroy(he_height);
    heman_image_destroy(he_albedo);

    m_bmp->SetValues(albedo_data);
}

heman_image* AlbedoMap::Baking(heman_image* height,
                               float min_height, float max_height)
{
    heman_image* grad = heman_color_create_gradient(
        256, COUNT(cp_colors), cp_locations, cp_colors);
    heman_image* albedo = heman_color_apply_gradient(height, min_height, max_height, grad);
    heman_image_destroy(grad);

    return albedo;
}

}
}