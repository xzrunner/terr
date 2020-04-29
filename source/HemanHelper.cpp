#include "terraingraph/HemanHelper.h"

#include <heightfield/HeightField.h>
#include <heightfield/Utility.h>

#include <assert.h>

namespace terraingraph
{

heman_image*
HemanHelper::Encode(const ur::Device& dev, const hf::HeightField& hf)
{
    size_t w = hf.Width();
    size_t h = hf.Height();

    auto he_height = heman_image_create(w, h, 1);

    // filling
    auto he_height_data = heman_image_data(he_height);
    auto& height_data = hf.GetValues(dev);
    for (size_t i = 0, n = height_data.size(); i < n; ++i) {
        he_height_data[i] = hf::Utility::HeightShortToFloat(height_data[i]);
    }

    return he_height;
}

std::shared_ptr<hf::HeightField>
HemanHelper::Decode(heman_image* img)
{
    int width, height, nbands;
    heman_image_info(img, &width, &height, &nbands);
    assert(nbands == 1);

    auto hf = std::make_shared<hf::HeightField>(width, height);

    // filling
    auto he_height_data = heman_image_data(img);
    std::vector<int32_t> height_data(width * height);
    for (size_t i = 0, n = height_data.size(); i < n; ++i) {
        height_data[i] = hf::Utility::HeightFloatToShort(he_height_data[i]);
    }

    hf->SetValues(height_data);

    return hf;
}

}