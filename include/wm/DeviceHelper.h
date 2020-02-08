#pragma once

#include "wm/typedef.h"

namespace wm
{

class HeightField;
class Bitmap;
class Mask;

class DeviceHelper
{
public:
    static DevicePtr GetInputDevice(const Device& dev, size_t idx);
    static std::shared_ptr<HeightField>
        GetInputHeight(const Device& dev, size_t idx);
    static std::shared_ptr<Bitmap>
        GetInputBitmap(const Device& dev, size_t idx);
    static std::shared_ptr<Mask>
        GetInputMask(const Device& dev, size_t idx);

}; // DeviceHelper

}