#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

#define Type terraingraph::device::Curves::Type

PARAM_INFO(CurveType,  Type,          type,     m_type,     (Type::Linear))
PARAM_INFO(CtrlPoints, Array<Float2>, ctrl_pts, m_ctrl_pts, ())

#undef Type