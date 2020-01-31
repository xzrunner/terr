#include "terr/TextureBaker.h"
#include "terr/HeightField.h"

namespace terr
{

ur::TexturePtr
TextureBaker::GenHeightMap(const HeightField& hf,
                           ur::RenderContext& rc,
                           const ur::TexturePtr& tex)
{
    if (hf.Width() == 0 || hf.Height() == 0) {
        return nullptr;
    }

    size_t w = hf.Width();
    size_t h = hf.Height();

    ur::TexturePtr ret = tex;
    if (!ret) {
        ret = std::make_shared<ur::Texture>();
    }

    std::vector<unsigned char> pixels;
    pixels.reserve(w * h);

    for (size_t y = 0; y < h; ++y) {
        for (size_t x = 0; x < w; ++x) {
            auto h = hf.Get(x, y);
            pixels.push_back(static_cast<unsigned char>(h * 255.0f));
        }
    }

    ret->Upload(&rc, hf.Width(), hf.Height(), ur::TEXTURE_A8, pixels.data());

    return ret;
}

}