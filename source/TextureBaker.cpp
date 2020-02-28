#include "wm/TextureBaker.h"
#include "wm/HeightField.h"
#include "wm/Bitmap.h"
#include "wm/Mask.h"
#include "wm/HemanHelper.h"
#include "wm/TextureGen.h"

#include <unirender/RenderContext.h>

#include <algorithm>

namespace wm
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
        const auto f01 = std::min(std::max(s_vals[i], 0.0f), 1.0f);
        dst[i] = static_cast<unsigned char>(f01 * 255.0f);
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
    ret->Upload(&rc, hf.Width(), hf.Height(), ur::TEXTURE_RED, pixels.data());

    return ret;
}

ur::TexturePtr
TextureBaker::GenColorMap(const Bitmap& bmp, ur::RenderContext& rc)
{
    auto& pixels = bmp.GetValues();
    if (pixels.empty()) {
        return nullptr;
    }

    ur::TexturePtr tex = nullptr;

    auto w = bmp.Width();
    auto h = bmp.Height();
    auto c = bmp.Channels();
    switch (c)
    {
    case 1:
    {
        assert(pixels.size() == w * h);
        int tex_id = rc.CreateTexture(pixels.data(), w, h, ur::TEXTURE_RED);
        tex = std::make_unique<ur::Texture>(&rc, w, h, ur::TEXTURE_RED, tex_id);
    }
        break;
    case 3:
    {
        assert(pixels.size() == w * h * 3);
        int tex_id = rc.CreateTexture(pixels.data(), w, h, ur::TEXTURE_RGB);
        tex = std::make_unique<ur::Texture>(&rc, w, h, ur::TEXTURE_RGB, tex_id);
    }
        break;
    default:
        assert(0);
    }

    return tex;
}

ur::TexturePtr
TextureBaker::GenColorMap(const Mask& mask, ur::RenderContext& rc)
{
    auto& flags = mask.GetValues();
    if (flags.empty()) {
        return nullptr;
    }

    auto w = mask.Width();
    auto h = mask.Height();
    assert(flags.size() == w * h);

    std::vector<unsigned char> pixels(w * h * 3, 255);
    for (size_t i = 0, n = w * h; i < n; ++i) {
        if (!flags[i]) {
            for (size_t j = 0; j < 3; ++j) {
                pixels[i * 3 + j] = 0;
            }
        }
    }
    int tex_id = rc.CreateTexture(pixels.data(), w, h, ur::TEXTURE_RGB);
    return std::make_unique<ur::Texture>(&rc, w, h, ur::TEXTURE_RGB, tex_id);
}

#ifdef USE_LSCAPE
//ur::TexturePtr
//TextureBaker::GenNormalMap(const HeightField& hf, ur::RenderContext& rc)
//{
//    auto w = hf.Width();
//    auto h = hf.Height();
//
//    std::vector<uint8_t> heights(w * h);
//    auto& val = hf.GetValues();
//    for (size_t i = 0, n = w * h; i < n; ++i) {
//        heights[i] = static_cast<uint8_t>(val[i] * 255);
//    }
//
//    float scale[] = { 1, 1, 1 };
//    //uint8_t* shadows = TextureGen::CalcNormals(
//    //    heights.data(), w, h, scale, 0.1f, 0.75f, 8
//    //);
//    uint8_t* shadows = TextureGen::CalcNormals(
//        heights.data(), w, h, scale, 0, 0, 0
//    );
//    int tex_id = rc.CreateTexture(shadows, w, h, ur::TEXTURE_RGB);
//    delete[] shadows;
//    return std::make_unique<ur::Texture>(&rc, w, h, ur::TEXTURE_RGB, tex_id);
//}
#else
ur::TexturePtr
TextureBaker::GenNormalMap(const HeightField& hf, ur::RenderContext& rc)
{
    auto w = hf.Width();
    auto h = hf.Height();
    std::vector<unsigned char> norm_data(w * h * 3, 255);

    auto he_height = HemanHelper::Encode(hf);

    auto he_norm = heman_lighting_compute_normals(he_height);
    auto he_norm_data = heman_image_data(he_norm);
    for (size_t i = 0, n = norm_data.size(); i < n; ++i) {
        const float flt01 = (he_norm_data[i] + 1) * 0.5f;
        norm_data[i] = static_cast<unsigned char>(flt01 * 255);
    }

    heman_image_destroy(he_height);
    heman_image_destroy(he_norm);

    int tex_id = rc.CreateTexture(norm_data.data(), w, h, ur::TEXTURE_RGB);
    return std::make_unique<ur::Texture>(&rc, w, h, ur::TEXTURE_RGB, tex_id);
}
#endif // USE_LSCAPE

ur::TexturePtr
TextureBaker::GenAmbientOcclusionMap(const HeightField& hf, ur::RenderContext& rc)
{
    auto w = hf.Width();
    auto h = hf.Height();
    std::vector<unsigned char> ao_data(w * h);

    auto he_height = HemanHelper::Encode(hf);

    auto he_ao = heman_lighting_compute_occlusion(he_height);
    auto he_ao_data = heman_image_data(he_ao);
    for (size_t i = 0, n = ao_data.size(); i < n; ++i) {
        ao_data[i] = static_cast<unsigned char>(he_ao_data[i] * 255);
    }

    heman_image_destroy(he_height);
    heman_image_destroy(he_ao);

    int tex_id = rc.CreateTexture(ao_data.data(), w, h, ur::TEXTURE_RED);
    return std::make_unique<ur::Texture>(&rc, w, h, ur::TEXTURE_RED, tex_id);
}

ur::TexturePtr
TextureBaker::GenShadowMap(const HeightField& hf, ur::RenderContext& rc, const sm::vec3& light_dir)
{
    auto w = hf.Width();
    auto h = hf.Height();

    std::vector<uint8_t> heights(w * h);
    auto& val = hf.GetValues();
    for (size_t i = 0, n = w * h; i < n; ++i) {
        heights[i] = static_cast<uint8_t>(val[i] * 255);
    }

    const float scale[] = { 1, 1, 1 };
    uint8_t* shadows = TextureGen::CalcShadows(
        heights.data(), w, h, scale, light_dir.xyz
    );

    std::vector<uint8_t> shadows_rgb(w * h * 3);
    for (size_t i = 0, n = w * h; i < n; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            shadows_rgb[i * 3 + j] = shadows[i];
        }
    }
    delete[] shadows;

    int tex_id = rc.CreateTexture(shadows_rgb.data(), w, h, ur::TEXTURE_RGB);
    return std::make_unique<ur::Texture>(&rc, w, h, ur::TEXTURE_RGB, tex_id);
}

}