#pragma once

#include <SM_Vector.h>

#include <vector>

namespace terr
{

class Bitmap
{
public:
    Bitmap(size_t width, size_t height);

    size_t Width() const { return m_width; }
    size_t Height() const { return m_height; }

    auto& GetValues() const { return m_values; }
    void SetValues(const std::vector<unsigned char>& values);

private:
    size_t m_width = 0, m_height = 0;

    std::vector<unsigned char> m_values;

}; // Bitmap

}