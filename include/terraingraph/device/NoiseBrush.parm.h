#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Radius, float,         radius, m_radius, (0.1f))
PARAM_INFO(Path,   std::vector<sm::vec3>, path,   m_path,   ())
