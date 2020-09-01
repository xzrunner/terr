#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Offsets, sm::vec2, offsets, m_offsets, (0, 0))

PARAM_INFO(Direction, float, direction,  m_direction, (0))
PARAM_INFO(Strengh,   float, strengh,    m_strengh,   (0))
PARAM_INFO(Strengh2,  float, strengh2,   m_strengh2,  (0))
