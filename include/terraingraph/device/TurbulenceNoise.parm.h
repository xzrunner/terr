#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Width,  int, width, m_width, (1024))
PARAM_INFO(Height, int, height, m_height, (1024))

PARAM_INFO(Type, terraingraph::device::TurbulenceNoise::Type, type, m_type, (terraingraph::device::TurbulenceNoise::Type::Perlin))

PARAM_INFO(UVScale,    float, uv_scale,   m_uv_scale,   (4))
PARAM_INFO(Seed,       float, seed,       m_seed,       (0))
PARAM_INFO(Lacunarity, float, lacunarity, m_lacunarity, (2))
PARAM_INFO(Gain,       float, gain,       m_gain,       (0.5))

PARAM_INFO(DistortionScale,   float, distortion_scale,    m_distortion_scale,    (0.15))
PARAM_INFO(DistortionGain,    float, distortion_gain,     m_distortion_gain,     (0.5))
PARAM_INFO(DistortionUVScale, float, distortion_uv_scale, m_distortion_uv_scale, (4.0))
PARAM_INFO(Warp,              float, warp,                m_warp,                (0.15))
PARAM_INFO(Gain1,             float, gain1,               m_gain1,               (0.8))
PARAM_INFO(Warp0,             float, warp0,               m_warp0,               (0.4))
PARAM_INFO(Damp0,             float, damp0,               m_damp0,               (1.0))
PARAM_INFO(Damp,              float, damp,                m_damp,                (0.8))
PARAM_INFO(DampScale,         float, damp_scale,          m_damp_scale,          (1.0))

PARAM_INFO(NoiseLargestFeat,   float, noise_largest_feat,   m_noise_largest_feat,   (200))
PARAM_INFO(NoiseCoarseRough,   float, noise_coarse_rough,   m_noise_coarse_rough,   (1))
PARAM_INFO(NoiseFineRough,     float, noise_fine_rough,     m_noise_fine_rough,     (1))
PARAM_INFO(NoiseStretchAngle,  float, noise_stretch_angle,  m_noise_stretch_angle,  (0))
PARAM_INFO(NoiseStretchRatio,  float, noise_stretch_ratio,  m_noise_stretch_ratio,  (1))

PARAM_INFO(DistortLargestFeat, float, distort_largest_feat, m_distort_largest_feat, (100))
PARAM_INFO(DistortCoarseRough, float, distort_coarse_rough, m_distort_coarse_rough, (1.2))
PARAM_INFO(DistortFineRough,   float, distort_fine_rough,   m_distort_fine_rough,   (0.8))
