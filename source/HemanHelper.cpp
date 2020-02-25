#include "wm/HemanHelper.h"
#include "wm/HeightField.h"

namespace wm
{

heman_image*
HemanHelper::Encode(const HeightField& hf)
{
    size_t w = hf.Width();
    size_t h = hf.Height();

    auto he_height = heman_image_create(w, h, 1);

    // filling
    auto he_height_data = heman_image_data(he_height);
    auto& height_data = hf.GetValues();
    for (size_t i = 0, n = height_data.size(); i < n; ++i) {
        he_height_data[i] = height_data[i];
    }

    return he_height;
}

}