#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Occlusion,        Float,  occlusion,         m_occlusion,         (1.0f))
PARAM_INFO(Diffuse,          Float,  diffuse,           m_diffuse,           (1.0f))
PARAM_INFO(DiffuseSoftening, Float,  diffuse_softening, m_diffuse_softening, (0.75f))
PARAM_INFO(LightPos,         Float3, light_pos,         m_light_pos,         (-0.5f, 0.5f, 1.0f))
