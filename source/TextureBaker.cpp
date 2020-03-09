#include "terraingraph/TextureBaker.h"
#include "terraingraph/Bitmap.h"
#include "terraingraph/Mask.h"
#include "terraingraph/HemanHelper.h"
#include "terraingraph/TextureGen.h"

#include <heightfield/HeightField.h>
#include <unirender/RenderContext.h>

#include <algorithm>

namespace terraingraph
{

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
//TextureBaker::GenNormalMap(const hf::HeightField& hf, ur::RenderContext& rc)
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
TextureBaker::GenNormalMap(const hf::HeightField& hf, ur::RenderContext& rc)
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
TextureBaker::GenAmbientOcclusionMap(const hf::HeightField& hf, ur::RenderContext& rc)
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
TextureBaker::GenShadowMap(const hf::HeightField& hf, ur::RenderContext& rc, const sm::vec3& light_dir)
{
    auto w = hf.Width();
    auto h = hf.Height();

    const float scale[] = { 1, 1, 1 };
    uint8_t* shadows = TextureGen::CalcShadows(
        hf.GetValues().data(), w, h, scale, light_dir.xyz
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