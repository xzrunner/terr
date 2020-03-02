#pragma once

#include "terraingraph/typedef.h"

namespace hf { class HeightField; }

namespace terraingraph
{

class Bitmap;
class Mask;

class DeviceHelper
{
public:
    static DevicePtr GetInputDevice(const Device& dev, size_t idx);
    static std::shared_ptr<hf::HeightField>
        GetInputHeight(const Device& dev, size_t idx);
    static std::shared_ptr<Bitmap>
        GetInputBitmap(const Device& dev, size_t idx);
    static std::shared_ptr<Mask>
        GetInputMask(const Device& dev, size_t idx);

}; // DeviceHelper

}