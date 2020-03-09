#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(LowerBound, Float, lower_bound, m_lower_bound, (1.0f))
PARAM_INFO(UpperBound, Float, upper_bound, m_upper_bound, (-1.0f))
