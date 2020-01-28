#pragma once

#include <unirender/Texture.h>

namespace ur { class RenderContext; }

namespace terr
{

class HeightField;

class TextureBaker
{
public:
    static ur::TexturePtr GenHeightMap(const HeightField& hf,
        ur::RenderContext& rc, const ur::TexturePtr& tex = nullptr);

}; // TextureBaker

}