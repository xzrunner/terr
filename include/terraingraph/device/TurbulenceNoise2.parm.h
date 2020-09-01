#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Frequency, float, frequency, m_frequency, (1.0f))
PARAM_INFO(Power,     float, power,     m_power,     (1.0f))
PARAM_INFO(Roughness, int,   roughness, m_roughness, (3))
PARAM_INFO(Seed,      int,   seed,      m_seed,      (0))
