#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(LightDir, Float3, light_dir, m_light_dir, { sm::vec3(1, 0, -1) })
