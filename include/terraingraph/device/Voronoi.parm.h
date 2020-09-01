#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Width,  size_t, width,  m_width,  (1024))
PARAM_INFO(Height, size_t, height, m_height, (1024))

PARAM_INFO(Displacement,   float, displacement, m_displacement, (1.0f))
PARAM_INFO(EnableDistance, bool,  enable_dist,  m_enable_dist,  (false))
PARAM_INFO(Frequency,      float, frequency,    m_frequency,    (1.0f))
PARAM_INFO(Seed,           int,   seed,         m_seed,         (0))
