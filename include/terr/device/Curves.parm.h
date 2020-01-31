#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(CurveType,  Type,          type,     m_type,     (Type::Linear))
PARAM_INFO(CtrlPoints, Array<Float2>, ctrl_pts, m_ctrl_pts, ())
