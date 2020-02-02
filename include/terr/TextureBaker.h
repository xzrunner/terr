#pragma once

#include <unirender/Texture.h>

namespace ur { class RenderContext; }

namespace terr
{

class HeightField;
class Bitmap;

class TextureBaker
{
public:
    static ur::TexturePtr GenHeightMap(const HeightField& hf,
        ur::RenderContext& rc, const ur::TexturePtr& tex = nullptr);

    static ur::TexturePtr GenColorMap(const Bitmap& bmp, ur::RenderContext& rc);

}; // TextureBaker

}