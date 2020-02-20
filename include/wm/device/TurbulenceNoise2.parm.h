#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Frequency, Float, frequency, m_frequency, (1.0f))
PARAM_INFO(Power,     Float, power,     m_power,     (1.0f))
PARAM_INFO(Roughness, Int,   roughness, m_roughness, (3))
PARAM_INFO(Seed,      Int,   seed,      m_seed,      (0))
