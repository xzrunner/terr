#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Width,  Int,   width,  m_width,  (128))
PARAM_INFO(Height, Int,   height, m_height, (128))

PARAM_INFO(RGB, Float3, rgb, m_rgb, ())
