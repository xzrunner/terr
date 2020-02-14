#pragma once

#include <SM_Vector.h>
#include <unirender/Texture.h>

#include <vector>

namespace wm
{

class HeightField
{
public:
    HeightField(size_t width, size_t height);

    bool Set(size_t x, size_t y, float h);
    float Get(size_t x, size_t y) const;
    float Get(float x, float y) const;
    float Get(size_t idx) const;

    bool Add(size_t x, size_t y, float dh);
    bool Add(size_t idx, float dh);

    bool Fill(const std::vector<float>& heights);

    void Scale(float scale);
    void Normalize();

    size_t Width() const { return m_width; }
    size_t Height() const { return m_height; }

    auto& GetValues() const { return m_values; }
    void SetValues(const std::vector<float>& values);

    ur::TexturePtr GetHeightmap();

    bool Inside(int x, int y) const;

private:
    size_t m_width = 0, m_height = 0;

    std::vector<float> m_values;

    ur::TexturePtr m_heightmap = nullptr;

}; // HeightField

}