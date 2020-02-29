#pragma once

#include <heman.h>

#include <memory>

namespace terraingraph
{

class HeightField;
class HemanHelper
{
public:
    static heman_image*
        Encode(const HeightField& hf);
    static std::shared_ptr<HeightField>
        Decode(heman_image* img);

}; // HemanHelper

}