#include "terraingraph/TextureBaker.h"
#include "terraingraph/Mask.h"
#include "terraingraph/HemanHelper.h"
#include "terraingraph/TextureGen.h"

#include <heightfield/HeightField.h>
#include <unirender2/TextureDescription.h>
#include <unirender2/Device.h>

#include <algorithm>

namespace terraingraph
{

ur2::TexturePtr
TextureBaker::GenColorMap(const Bitmap& bmp, const ur2::Device& dev)
{
    auto pixels = bmp.GetPixels();

    ur2::TexturePtr tex = nullptr;

    auto w = bmp.Width();
    auto h = bmp.Height();
    auto c = bmp.Channels();

    ur2::TextureDescription desc;
    desc.target = ur2::TextureTarget::Texture2D;
    desc.width = w;
    desc.height = h;
    switch (c)
    {
    case 1:
    {
        desc.format = ur2::TextureFormat::RED;
        tex = dev.CreateTexture(desc, pixels);
    }
        break;
    case 3:
    {
        desc.format = ur2::TextureFormat::RGB;
        tex = dev.CreateTexture(desc, pixels);
    }
        break;
    default:
        assert(0);
    }

    return tex;
}

ur2::TexturePtr
TextureBaker::GenColorMap(const Mask& mask, const ur2::Device& dev)
{
    auto flags = mask.GetPixels();

    auto w = mask.Width();
    auto h = mask.Height();
    assert(mask.Channels() == 1);

    std::vector<unsigned char> pixels(w * h * 3, 255);
    for (size_t i = 0, n = w * h; i < n; ++i) {
        if (!flags[i]) {
            for (size_t j = 0; j < 3; ++j) {
                pixels[i * 3 + j] = 0;
            }
        }
    }

    ur2::TextureDescription desc;
    desc.target = ur2::TextureTarget::Texture2D;
    desc.width = w;
    desc.height = h;
    desc.format = ur2::TextureFormat::RGB;
    return dev.CreateTexture(desc, pixels.data());
}

#ifdef USE_LSCAPE
//ur2::TexturePtr
//TextureBaker::GenNormalMap(const hf::HeightField& hf, const ur2::Device& dev)
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
ur2::TexturePtr
TextureBaker::GenNormalMap(const hf::HeightField& hf, const ur2::Device& dev)
{
    auto w = hf.Width();
    auto h = hf.Height();
    std::vector<unsigned char> norm_data(w * h * 3, 255);

    auto he_height = HemanHelper::Encode(dev, hf);

    auto he_norm = heman_lighting_compute_normals(he_height);
    auto he_norm_data = heman_image_data(he_norm);
    for (size_t i = 0, n = norm_data.size(); i < n; ++i) {
        const float flt01 = (he_norm_data[i] + 1) * 0.5f;
        norm_data[i] = static_cast<unsigned char>(flt01 * 255);
    }

    heman_image_destroy(he_height);
    heman_image_destroy(he_norm);

    ur2::TextureDescription desc;
    desc.target = ur2::TextureTarget::Texture2D;
    desc.width = w;
    desc.height = h;
    desc.format = ur2::TextureFormat::RGB;
    return dev.CreateTexture(desc, norm_data.data());
}
#endif // USE_LSCAPE

ur2::TexturePtr
TextureBaker::GenAmbientOcclusionMap(const hf::HeightField& hf, const ur2::Device& dev)
{
    auto w = hf.Width();
    auto h = hf.Height();
    std::vector<unsigned char> ao_data(w * h);

    auto he_height = HemanHelper::Encode(dev, hf);

    auto he_ao = heman_lighting_compute_occlusion(he_height);
    auto he_ao_data = heman_image_data(he_ao);
    for (size_t i = 0, n = ao_data.size(); i < n; ++i) {
        ao_data[i] = static_cast<unsigned char>(he_ao_data[i] * 255);
    }

    heman_image_destroy(he_height);
    heman_image_destroy(he_ao);

    ur2::TextureDescription desc;
    desc.target = ur2::TextureTarget::Texture2D;
    desc.width = w;
    desc.height = h;
    desc.format = ur2::TextureFormat::RED;
    return dev.CreateTexture(desc, ao_data.data());
}

ur2::TexturePtr
TextureBaker::GenShadowMap(const hf::HeightField& hf, const ur2::Device& dev, const sm::vec3& light_dir)
{
    auto w = hf.Width();
    auto h = hf.Height();

    const float scale[] = { 1, 1, 1 };
    uint8_t* shadows = TextureGen::CalcShadows(
        hf.GetValues(dev).data(), w, h, scale, light_dir.xyz
    );

    std::vector<uint8_t> shadows_rgb(w * h * 3);
    for (size_t i = 0, n = w * h; i < n; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            shadows_rgb[i * 3 + j] = shadows[i];
        }
    }
    delete[] shadows;

    ur2::TextureDescription desc;
    desc.target = ur2::TextureTarget::Texture2D;
    desc.width = w;
    desc.height = h;
    desc.format = ur2::TextureFormat::RGB;
    return dev.CreateTexture(desc, shadows_rgb.data());
}

}