#pragma once

#include "terraingraph/typedef.h"

#include <unirender2/typedef.h>
#include <SM_Vector.h>

#include <vector>

namespace hf { class HeightField; }
namespace ur2 { class Device; }

namespace terraingraph
{

class Mask;

class TextureBaker
{
public:
    static ur2::TexturePtr GenColorMap(const Bitmap& bmp, const ur2::Device& dev);
    static ur2::TexturePtr GenColorMap(const Mask& mask, const ur2::Device& dev);

    static ur2::TexturePtr GenNormalMap(const hf::HeightField& hf, const ur2::Device& dev);
    static ur2::TexturePtr GenAmbientOcclusionMap(const hf::HeightField& hf, const ur2::Device& dev);
    static ur2::TexturePtr GenShadowMap(const hf::HeightField& hf, const ur2::Device& dev, const sm::vec3& light_dir);

}; // TextureBaker

}