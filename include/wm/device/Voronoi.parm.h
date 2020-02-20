#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Width,  Int,   width,  m_width,  (512))
PARAM_INFO(Height, Int,   height, m_height, (512))

PARAM_INFO(Displacement,   Float, displacement, m_displacement, (1.0f))
PARAM_INFO(EnableDistance, Bool,  enable_dist,  m_enable_dist,  (false))
PARAM_INFO(Frequency,      Float, frequency,    m_frequency,    (1.0f))
PARAM_INFO(Seed,           Int,   seed,         m_seed,         (0))
