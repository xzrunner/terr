#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Width,  size_t, width,  m_width,  (1024))
PARAM_INFO(Height, size_t, height, m_height, (1024))

PARAM_INFO(Frequency,  float, frequency,  m_frequency,  (4.0f))
PARAM_INFO(Amplitude,  float, amplitude,  m_amplitude,  (0.5f))
PARAM_INFO(Octaves,    int,   octaves,    m_octaves,    (3))
PARAM_INFO(Lacunarity, float, lacunarity, m_lacunarity, (2.0f))
PARAM_INFO(Gain,       float, gain,       m_gain,       (0.5f))
PARAM_INFO(Seed,       int,   seed,       m_seed,       (0))
