#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

#define Method terraingraph::device::Combiner::Method

PARAM_INFO(MethodType, Method, method, m_method, (Method::Average))

#undef Method