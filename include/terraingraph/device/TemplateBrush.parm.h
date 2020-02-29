#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Rotate,    Float,          rotate,    m_rotate,    (0.0f))
PARAM_INFO(Scale,     Float,          scale,     m_scale,     (1.0f))
PARAM_INFO(Filepath,  String,         filepath,  m_filepath,  ())
PARAM_INFO(Matrixes,  Array<Matrix4>, matrixes,  m_matrixes,  ())