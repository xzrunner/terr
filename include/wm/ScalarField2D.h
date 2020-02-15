#pragma once

#include <vector>

namespace wm
{

struct ScalarValue
{
    ScalarValue() { }
    ScalarValue(int a, int b, float h) : x(a), y(b), value(h) { }

    size_t x, y;
	float value;
};

class ScalarField2D
{
public:
    ScalarField2D(size_t width, size_t height);
    ScalarField2D(size_t width, size_t height, float val);

    size_t Width() const { return m_width; }
    size_t Height() const { return m_height; }

    auto& GetValues() const { return m_values; }
    void SetValues(const std::vector<float>& values);

    bool Set(size_t x, size_t y, float h);
    float Get(size_t x, size_t y) const;
    float Get(float x, float y) const;
    float Get(size_t idx) const;

    bool Add(size_t x, size_t y, float dh);
    bool Add(size_t idx, float dh);

    bool Inside(int x, int y) const;

    void Scale(float scale);

    void Normalize();

protected:
    size_t m_width = 0, m_height = 0;

    std::vector<float> m_values;

    bool m_dirty = true;

}; // ScalarField2D

}