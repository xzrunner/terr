#include "wm/Bitmap.h"

namespace wm
{

Bitmap::Bitmap(size_t width, size_t height, size_t channels)
    : m_width(width)
    , m_height(height)
    , m_channels(channels)
{
    m_values.resize(m_width * m_height * channels, 255);
}

void Bitmap::SetValues(const std::vector<unsigned char>& values)
{
    if (m_width * m_height * 3 != values.size()) {
        return;
    }

    assert(m_values.size() == values.size());
    m_values = values;
}

}