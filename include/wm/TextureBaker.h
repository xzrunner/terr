#pragma once

#include <unirender/Texture.h>

#include <SM_Vector.h>

#include <vector>

namespace ur { class RenderContext; }

namespace wm
{

class HeightField;
class Bitmap;
class Mask;

class TextureBaker
{
public:
    static bool GenHeightMap(const HeightField& src,
        std::vector<unsigned char>& dst);
    static ur::TexturePtr GenHeightMap(const HeightField& hf,
        ur::RenderContext& rc, const ur::TexturePtr& tex = nullptr);

    static ur::TexturePtr GenColorMap(const Bitmap& bmp, ur::RenderContext& rc);
    static ur::TexturePtr GenColorMap(const Mask& mask, ur::RenderContext& rc);

    static ur::TexturePtr GenNormalMap(const HeightField& hf, ur::RenderContext& rc);
    static ur::TexturePtr GenAmbientOcclusionMap(const HeightField& hf, ur::RenderContext& rc);
    static ur::TexturePtr GenShadowMap(const HeightField& hf, ur::RenderContext& rc, const sm::vec3& light_dir);

}; // TextureBaker

}