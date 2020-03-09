#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Width,  size_t, width,  m_width,  (1024))
PARAM_INFO(Height, size_t, height, m_height, (1024))

PARAM_INFO(Points,   Array<Float3>, points,    m_points,    ({ sm::vec3(0.5f, 0.4f, 0.4f), sm::vec3(0.3f, 0.5f, 0.6f), sm::vec3(0.7f, 0.7f, 0.2f) }))
PARAM_INFO(NoiseAmt, Float,         noise_amt, m_noise_amt, (0.3f))
PARAM_INFO(Seed,     Int,           seed,      m_seed,      (0))
