#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Gradient, Array<Float4>, gradient, m_gradient, ({ sm::vec4(0, 0, 0, 0), sm::vec4(1, 1, 1, 1) }))
