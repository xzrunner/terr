#pragma once

#include <unirender/Texture.h>

#include <vector>

namespace ur { class RenderContext; }

namespace terr
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

}; // TextureBaker

}