#pragma once

#include <heman.h>

namespace wm
{

class HeightField;
class HemanHelper
{
public:
    static heman_image*
        Encode(const HeightField& hf);

}; // HemanHelper

}