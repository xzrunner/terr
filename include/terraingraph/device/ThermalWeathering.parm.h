#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Iterations,        Int,   iter,                m_iter,                (1))
PARAM_INFO(Amplitude,         Int,   amplitude,           m_amplitude,           (1))
PARAM_INFO(TanThresholdAngle, Float, tan_threshold_angle, m_tan_threshold_angle, (0.6))
