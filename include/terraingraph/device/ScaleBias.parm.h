#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Bias,  int,   bias,  m_bias,  (0))
PARAM_INFO(Scale, float, scale, m_scale, (1.0f))
