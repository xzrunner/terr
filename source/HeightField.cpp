#include "wm/HeightField.h"
#include "wm/TextureBaker.h"

#include <unirender/Blackboard.h>
#include <unirender/RenderContext.h>

#include <assert.h>

namespace wm
{

HeightField::HeightField(size_t width, size_t height)
    : m_width(width)
    , m_height(height)
{
    m_values.resize(m_width * m_height, 0);
}

bool HeightField::Set(size_t x, size_t y, float h)
{
    const size_t idx = y * m_width + x;
    if (idx >= m_values.size()) {
        assert(0);
        return false;
    } else {
        m_values[idx] = h;
        return true;
    }
}

float HeightField::Get(size_t x, size_t y) const
{
    const size_t idx = y * m_width + x;
    if (idx >= m_values.size()) {
        assert(0);
        return 0;
    } else {
        return m_values[idx];
    }
}

float HeightField::Get(float x, float y) const
{
    if (x < 0 || x >= m_width ||
        y < 0 || y >= m_height) {
        assert(0);
        return 0;
    }

    const size_t ix = static_cast<size_t>(std::floor(x));
    const size_t iy = static_cast<size_t>(std::floor(y));
    const size_t nix = (ix == m_width - 1) ? ix : ix + 1;
    const size_t niy = (iy == m_height - 1) ? iy : iy + 1;
    const float h00 = Get(ix,  iy);
    const float h10 = Get(nix, iy);
    const float h01 = Get(ix,  niy);
    const float h11 = Get(nix, niy);

    const float dx = x - ix;
    const float dy = y - iy;
    return (h00 * (1 - dx) + h10 * dx) * (1 - dy) + (h01 * (1 - dx) + h11 * dx) * dy;
}

float HeightField::Get(size_t idx) const
{
    if (idx >= m_values.size()) {
        assert(0);
        return 0;
    } else {
        return m_values[idx];
    }
}

bool HeightField::Add(size_t x, size_t y, float dh)
{
    if (x < 0 || x >= m_width ||
        y < 0 || y >= m_height) {
        return false;
    }

    const size_t idx = y * m_width + x;
    m_values[idx] += dh;

    return true;
}

bool HeightField::Add(size_t idx, float dh)
{
    if (idx >= m_values.size()) {
        assert(0);
        return false;
    }

    m_values[idx] += dh;

    return true;
}

bool HeightField::Fill(const std::vector<float>& heights)
{
    if (m_values.size() != heights.size()) {
        assert(0);
        return false;
    } else {
        m_values = heights;
        return true;
    }
}

void HeightField::Scale(float scale)
{
    for (auto& v : m_values) {
        v *= scale;
    }
}

void HeightField::Normalize()
{
    float min = std::numeric_limits<float>::max();
    float max = std::numeric_limits<float>::min();
    for (auto& v : m_values)
    {
        if (v < min) {
            min = v;
        }
        if (v > max) {
            max = v;
        }
    }
    if (min == max) {
        return;
    }

    for (auto& v : m_values) {
        v = (v - min) / (max - min);
    }
}

void HeightField::SetValues(const std::vector<float>& values)
{
    if (m_width * m_height != values.size()) {
        return;
    }

    assert(m_values.size() == values.size());
    m_values = values;
}

ur::TexturePtr HeightField::GetHeightmap()
{
    if (!m_heightmap) {
        auto& rc = ur::Blackboard::Instance()->GetRenderContext();
        m_heightmap = TextureBaker::GenHeightMap(*this, rc);
    }
    return m_heightmap;
}

}