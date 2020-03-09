#pragma once

#include <primitive/Bitmap.h>

#include <memory>

namespace terraingraph
{

class Device;
using DevicePtr = std::shared_ptr<Device>;

using Bitmap = prim::Bitmap<unsigned char>;
using BitmapPtr = std::shared_ptr<Bitmap>;

//using Mash   = prim::Bitmap<bool>;

}