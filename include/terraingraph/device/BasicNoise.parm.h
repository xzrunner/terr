#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Width,  Int,   width,  m_width,  (1024))
PARAM_INFO(Height, Int,   height, m_height, (1024))

PARAM_INFO(Persistence, Float, persistence, m_persistence, (2))
PARAM_INFO(Frequency,   Float, frequency,   m_frequency,   (0.01))
PARAM_INFO(Amplitude,   Float, amplitude,   m_amplitude,   (0.5))
PARAM_INFO(Octaves,     Int,   octaves,     m_octaves,     (2))
PARAM_INFO(Randomseed,  Int,   randomseed,  m_randomseed,  (0))
