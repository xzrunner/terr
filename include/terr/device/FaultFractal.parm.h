#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Iterations, Int,   iterations, m_iter,      (64))
PARAM_INFO(MinDelta,   Int,   min_delta,  m_min_delta, (0))
PARAM_INFO(MaxDelta,   Int,   max_delta,  m_max_delta, (255))
PARAM_INFO(Filter,     Float, filter,     m_filter,    (0.05f))
