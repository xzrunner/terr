#include "terr/TextureBaker.h"
#include "terr/HeightField.h"
#include "terr/Bitmap.h"

#include <unirender/RenderContext.h>

namespace terr
{

bool TextureBaker::GenHeightMap(const HeightField& src,
                                std::vector<unsigned char>& dst)
{
    if (src.Width() == 0 || src.Height() == 0) {
        return false;
    }

    auto& s_vals = src.GetValues();
    dst.resize(s_vals.size());
    for (size_t i = 0, n = s_vals.size(); i < n; ++i) {
        dst[i] = static_cast<unsigned char>(s_vals[i] * 255.0f);
    }

    return true;
}

ur::TexturePtr
TextureBaker::GenHeightMap(const HeightField& hf,
                           ur::RenderContext& rc,
                           const ur::TexturePtr& tex)
{
    if (hf.Width() == 0 || hf.Height() == 0) {
        return nullptr;
    }

    ur::TexturePtr ret = tex;
    if (!ret) {
        ret = std::make_shared<ur::Texture>();
    }

    std::vector<unsigned char> pixels;
    if (!GenHeightMap(hf, pixels)) {
        return nullptr;
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