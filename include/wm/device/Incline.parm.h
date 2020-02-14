#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Offsets, Float2, offsets, m_offsets, (0, 0))

PARAM_INFO(Offset, Float, offset, m_offset, (0))
PARAM_INFO(Factor, Float, factor, m_factor, (0))
PARAM_INFO(Invert, Bool,  invert, m_invert, (false))
