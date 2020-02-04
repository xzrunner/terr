#include "terr/Mask.h"

namespace terr
{

Mask::Mask(size_t width, size_t height)
    : m_width(width)
    , m_height(height)
{
    m_values.resize(m_width * m_height, false);
}

void Mask::SetValues(const std::vector<bool>& values)
{
    assert(m_values.size() == values.size());
    m_values = values;
}

}