#pragma once

#include <primitive/Bitmap.h>
#include <SM_Vector.h>

namespace terraingraph
{

class Mask : public prim::Bitmap<bool>
{
public:
    Mask(size_t width, size_t height);

    std::vector<std::vector<sm::ivec2>>
        CalcBorders() const;

    bool IsPixelBorder(size_t x, size_t y) const;

private:
    bool IsPixelMasked(size_t x, size_t y) const;

}; // Mask

}