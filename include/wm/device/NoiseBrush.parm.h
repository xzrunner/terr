#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Radius, Float,         radius, m_radius, (0.1f))
PARAM_INFO(Path,   Array<Float3>, path,   m_path,   ())
