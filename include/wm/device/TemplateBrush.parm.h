#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Translate, Float2,        translate, m_translate, (0.0f, 0.0f))
PARAM_INFO(Rotate,    Float,         rotate,    m_rotate,    (0.0f))
PARAM_INFO(Scale,     Float2,        scale,     m_scale,     (1.0f, 1.0f))
PARAM_INFO(Filepath,  String,        filepath,  m_filepath,  ())
PARAM_INFO(Positions, Array<Float2>, positions, m_positions, ())