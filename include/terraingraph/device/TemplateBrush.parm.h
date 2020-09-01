#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Rotate,    float,          rotate,    m_rotate,    (0.0f))
PARAM_INFO(Scale,     float,          scale,     m_scale,     (1.0f))
PARAM_INFO(Filepath,  std::string,         filepath,  m_filepath,  ())
PARAM_INFO(Matrixes,  std::vector<sm::mat4>, matrixes,  m_matrixes,  ())