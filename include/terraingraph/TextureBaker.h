#pragma once

#include "terraingraph/typedef.h"

#include <unirender/typedef.h>
#include <SM_Vector.h>

#include <vector>

namespace hf { class HeightField; }
namespace ur { class Device; }

namespace terraingraph
{

class Mask;

class TextureBaker
{
public:
    static ur::TexturePtr GenColorMap(const Bitmap& bmp, const ur::Device& dev);
    static ur::TexturePtr GenColorMap(const Mask& mask, const ur::Device& dev);

    static ur::TexturePtr GenNormalMap(const hf::HeightField& hf, const ur::Device& dev);
    static ur::TexturePtr GenAmbientOcclusionMap(const hf::HeightField& hf, const ur::Device& dev);
    static ur::TexturePtr GenShadowMap(const hf::HeightField& hf, const ur::Device& dev, const sm::vec3& light_dir);

}; // TextureBaker

}