#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Width,  Int,   width,  m_width,  (512))
PARAM_INFO(Height, Int,   height, m_height, (512))

PARAM_INFO(Resolution, Float2, resolution, m_resolution, (1, 1))
PARAM_INFO(Seed,       Float,  seed,       m_seed,       (0))
