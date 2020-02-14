#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Offsets, Float2, offsets, m_offsets, (0, 0))

PARAM_INFO(Invert,  Bool,   invert,  m_invert,  (false))
PARAM_INFO(Shallow, Bool,   shallow, m_shallow, (false))
PARAM_INFO(Rough,   Bool,   rough,   m_rough,   (false))
PARAM_INFO(Slope,   Bool,   slope,   m_slope,   (false))
