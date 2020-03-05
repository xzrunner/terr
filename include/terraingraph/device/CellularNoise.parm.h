#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Width,  Int,   width,  m_width,  (1024))
PARAM_INFO(Height, Int,   height, m_height, (1024))

PARAM_INFO(Offset, Float2, offset, m_offset, (0, 0))

PARAM_INFO(Resolution, Float2, resolution, m_resolution, (1, 1))
PARAM_INFO(Seed,       Float,  seed,       m_seed,       (0))
