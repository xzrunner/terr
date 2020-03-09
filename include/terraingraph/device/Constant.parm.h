#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Width,  size_t, width,  m_width,  (1024))
PARAM_INFO(Height, size_t, height, m_height, (1024))

PARAM_INFO(Value, Int, value, m_value, (0))
