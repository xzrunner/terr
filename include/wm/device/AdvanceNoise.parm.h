#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Width, Int, width, m_width, (128))
PARAM_INFO(Height, Int, height, m_height, (128))

PARAM_INFO(Type, wm::device::AdvanceNoise::Type, type, m_type, (wm::device::AdvanceNoise::Type::Perlin))

PARAM_INFO(UVScale,    Float, uv_scale,   m_uv_scale,   (4))
PARAM_INFO(Seed,       Float, seed,       m_seed,       (0))
PARAM_INFO(Lacunarity, Float, lacunarity, m_lacunarity, (2))
PARAM_INFO(Gain,       Float, gain,       m_gain,       (0.5))

PARAM_INFO(DistortionScale,   Float, distortion_scale,    m_distortion_scale,    (0.15))
PARAM_INFO(DistortionGain,    Float, distortion_gain,     m_distortion_gain,     (0.5))
PARAM_INFO(DistortionUVScale, Float, distortion_uv_scale, m_distortion_uv_scale, (4.0))
PARAM_INFO(Warp,              Float, warp,                m_warp,                (0.15))
PARAM_INFO(Gain1,             Float, gain1,               m_gain1,               (0.8))
PARAM_INFO(Warp0,             Float, warp0,               m_warp0,               (0.4))
PARAM_INFO(Damp0,             Float, damp0,               m_damp0,               (1.0))
PARAM_INFO(Damp,              Float, damp,                m_damp,                (0.8))
PARAM_INFO(DampScale,         Float, damp_scale,          m_damp_scale,          (1.0))
