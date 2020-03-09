#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Width,  size_t, width,  m_width,  (1024))
PARAM_INFO(Height, size_t, height, m_height, (1024))

PARAM_INFO(Frequency,  Float, frequency,  m_frequency,  (4.0f))
PARAM_INFO(Amplitude,  Float, amplitude,  m_amplitude,  (0.5f))
PARAM_INFO(Octaves,    Int,   octaves,    m_octaves,    (3))
PARAM_INFO(Lacunarity, Float, lacunarity, m_lacunarity, (2.0f))
PARAM_INFO(Gain,       Float, gain,       m_gain,       (0.5f))
PARAM_INFO(Seed,       Int,   seed,       m_seed,       (0))
