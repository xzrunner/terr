#include "terr/Bitmap.h"

namespace terr
{

Bitmap::Bitmap(size_t width, size_t height)
    : m_width(width)
    , m_height(height)
{
    m_values.resize(m_width * m_height * 3, 255);
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