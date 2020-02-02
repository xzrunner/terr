#include "terr/TextureBaker.h"
#include "terr/HeightField.h"
#include "terr/Bitmap.h"

#include <unirender/RenderContext.h>

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

ur::TexturePtr
TextureBaker::GenColorMap(const Bitmap& bmp, ur::RenderContext& rc)
{
    auto& pixels = bmp.GetValues();
    if (pixels.empty()) {
        return nullptr;
    }

    auto w = bmp.Width();
    auto h = bmp.Height();
    assert(pixels.size() == w * h * 3);

    int tex_id = rc.CreateTexture(pixels.data(), w, h, ur::TEXTURE_RGB);
    return std::make_unique<ur::Texture>(&rc, w, h, ur::TEXTURE_RGB, tex_id);
}

}