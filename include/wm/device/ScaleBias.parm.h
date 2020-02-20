#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Bias,  Float, bias,  m_bias,  (0.0f))
PARAM_INFO(Scale, Float, scale, m_scale, (1.0f))
