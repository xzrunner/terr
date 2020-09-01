#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Translate, sm::vec2, translate, m_translate, (0.0f, 0.0f))
PARAM_INFO(Rotate,    float,  rotate,    m_rotate,    (0.0f))
PARAM_INFO(Scale,     sm::vec2, scale,     m_scale,     (1.0f, 1.0f))
