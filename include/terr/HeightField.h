#pragma once

#include <SM_Vector.h>

#include <vector>

namespace terr
{

class HeightField
{
public:
    HeightField(size_t x, size_t y);

    bool Set(size_t x, size_t y, float h);
    float Get(size_t x, size_t y) const;
    float Get(float x, float y) const;
    float Get(size_t idx) const;

    bool Add(size_t x, size_t y, float dh);
    bool Add(size_t idx, float dh);

    bool Fill(const std::vector<float>& heights);

    void Scale(float scale);
    void Normalize();

    size_t Width() const { return m_x; }
    size_t Height() const { return m_y; }

private:
    size_t m_x = 0, m_y = 0;

    std::vector<float> m_values;

}; // HeightField

}