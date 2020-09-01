#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Offsets, sm::vec2, offsets, m_offsets, (0, 0))

PARAM_INFO(Offset, float, offset, m_offset, (0))
PARAM_INFO(Factor, float, factor, m_factor, (0))
PARAM_INFO(Invert, bool,  invert, m_invert, (false))
