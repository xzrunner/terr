#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Width,  size_t, width,  m_width,  (1024))
PARAM_INFO(Height, size_t, height, m_height, (1024))

PARAM_INFO(Persistence, float, persistence, m_persistence, (2))
PARAM_INFO(Frequency,   float, frequency,   m_frequency,   (0.01))
PARAM_INFO(Amplitude,   float, amplitude,   m_amplitude,   (0.5))
PARAM_INFO(Octaves,     int,   octaves,     m_octaves,     (2))
PARAM_INFO(Randomseed,  int,   randomseed,  m_randomseed,  (0))
