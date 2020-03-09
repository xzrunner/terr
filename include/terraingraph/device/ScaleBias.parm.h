#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Bias,  Int,   bias,  m_bias,  (0))
PARAM_INFO(Scale, Float, scale, m_scale, (1.0f))
