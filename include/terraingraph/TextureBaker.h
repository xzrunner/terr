#pragma once

#include <unirender/Texture.h>

#include <SM_Vector.h>

#include <vector>

namespace ur { class RenderContext; }
namespace hf { class HeightField; }

namespace terraingraph
{

class Bitmap;
class Mask;

class TextureBaker
{
public:
    static bool GenHeightMap(const hf::HeightField& src, std::vector<short>& dst);
    static ur::TexturePtr GenHeightMap(const hf::HeightField& hf,
        ur::RenderContext& rc, const ur::TexturePtr& tex = nullptr);

    static ur::TexturePtr GenColorMap(const Bitmap& bmp, ur::RenderContext& rc);
    static ur::TexturePtr GenColorMap(const Mask& mask, ur::RenderContext& rc);

    static ur::TexturePtr GenNormalMap(const hf::HeightField& hf, ur::RenderContext& rc);
    static ur::TexturePtr GenAmbientOcclusionMap(const hf::HeightField& hf, ur::RenderContext& rc);
    static ur::TexturePtr GenShadowMap(const hf::HeightField& hf, ur::RenderContext& rc, const sm::vec3& light_dir);

}; // TextureBaker

}