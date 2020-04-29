#pragma once

#include <heman.h>

#include <memory>

namespace hf { class HeightField; }
namespace ur { class Device; }

namespace terraingraph
{

class HemanHelper
{
public:
    static heman_image*
        Encode(const ur::Device& dev, const hf::HeightField& hf);
    static std::shared_ptr<hf::HeightField>
        Decode(heman_image* img);

}; // HemanHelper

}