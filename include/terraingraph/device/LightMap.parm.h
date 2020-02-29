#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(LightDir, Float3, light_dir, m_light_dir, { sm::vec3(1, 0, -1) })
PARAM_INFO(Ambient,  Float,  ambient,   m_ambient,   (0.5f))
PARAM_INFO(Diffuse,  Float,  diffuse,   m_diffuse,   (0.5f))
