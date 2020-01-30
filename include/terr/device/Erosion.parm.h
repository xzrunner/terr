#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Iterations,             Int,   iter,      m_iter,      (5))
PARAM_INFO(ErosionRadius,          Float, radius,    m_radius,    (3))

PARAM_INFO(SedimentCapacityFactor, Float, kq,        m_kq,        (10))
PARAM_INFO(MinSedimentCapacity,    Float, min_slope, m_min_slope, (0.05))
PARAM_INFO(EvaporateSpeed,         Float, kw,        m_kw,        (0.001))
PARAM_INFO(ErodeSpeed,             Float, kr,        m_kr,        (0.9))
PARAM_INFO(DepositSpeed,           Float, kd,        m_kd,        (0.02))
PARAM_INFO(Inertia,                Float, ki,        m_ki,        (0.1))
PARAM_INFO(Gravity,                Float, g,         m_g,         (20))
