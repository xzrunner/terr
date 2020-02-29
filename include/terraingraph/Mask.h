#pragma once

#include <SM_Vector.h>

#include <vector>

namespace terraingraph
{

class Mask
{
public:
    Mask(size_t width, size_t height);

    size_t Width() const { return m_width; }
    size_t Height() const { return m_height; }

    auto& GetValues() const { return m_values; }
    void SetValues(const std::vector<bool>& values);

    std::vector<std::vector<sm::ivec2>>
        CalcBorders() const;

    bool IsPixelBorder(size_t x, size_t y) const;

private:
    bool IsPixelMasked(size_t x, size_t y) const;

private:
    size_t m_width = 0, m_height = 0;

    std::vector<bool> m_values;

}; // Mask

}