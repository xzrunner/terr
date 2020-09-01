#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Iterations,             int,   iter,      m_iter,      (10))
PARAM_INFO(ErosionRadius,          float, radius,    m_radius,    (3))      // 2, 8

PARAM_INFO(SedimentCapacityFactor, float, kq,        m_kq,        (4))
PARAM_INFO(MinSedimentCapacity,    float, min_slope, m_min_slope, (0.01))
PARAM_INFO(EvaporateSpeed,         float, kw,        m_kw,        (0.01))   // 0, 1
PARAM_INFO(ErodeSpeed,             float, kr,        m_kr,        (0.3))    // 0, 1
PARAM_INFO(DepositSpeed,           float, kd,        m_kd,        (0.3))    // 0, 1
PARAM_INFO(Inertia,                float, ki,        m_ki,        (0.05))   // 0, 1
PARAM_INFO(Gravity,                float, g,         m_g,         (4))
