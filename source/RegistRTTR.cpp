#define EXE_FILEPATH "wm/device_include_gen.h"
#include "wm/device_regist_cfg.h"
#undef EXE_FILEPATH

#include <rttr/registration>

#define REGIST_NODE_TYPE(type, name)                            \
	rttr::registration::class_<wm::device::type>("wm::"#name) \
		.constructor<>()                                        \
	;

#define REGIST_ENUM_ITEM(type, name, label) \
    rttr::value(name, type),                \
    rttr::metadata(type, label)             \

RTTR_REGISTRATION
{

rttr::registration::class_<dag::Node<wm::DeviceVarType>::Port>("wm::Operation::Port")
	.property("var", &dag::Node<wm::DeviceVarType>::Port::var)
;

rttr::registration::class_<wm::Device>("wm::Device")
	.method("GetImports", &wm::Device::GetImports)
	.method("GetExports", &wm::Device::GetExports)
;

#define EXE_FILEPATH "wm/device_rttr_gen.h"
#include "wm/device_regist_cfg.h"
#undef EXE_FILEPATH

rttr::registration::enumeration<wm::device::Combiner::Method> ("terr_combiner_method")
(
    REGIST_ENUM_ITEM(wm::device::Combiner::Method::Average,  "average",  "Average"),
    REGIST_ENUM_ITEM(wm::device::Combiner::Method::Add,      "add",      "Add"),
    REGIST_ENUM_ITEM(wm::device::Combiner::Method::Subtract, "subtract", "Subtract"),
    REGIST_ENUM_ITEM(wm::device::Combiner::Method::Multiply, "multiply", "Multiply"),
    REGIST_ENUM_ITEM(wm::device::Combiner::Method::Divide,   "divide",   "Divide"),
    REGIST_ENUM_ITEM(wm::device::Combiner::Method::Max,      "max",      "Max"),
    REGIST_ENUM_ITEM(wm::device::Combiner::Method::Min,      "min",      "Min")
);

rttr::registration::enumeration<wm::device::Curves::Type>("terr_curves_type")
(
    REGIST_ENUM_ITEM(wm::device::Curves::Type::Linear, "linear", "Linear"),
    REGIST_ENUM_ITEM(wm::device::Curves::Type::Spline, "spline", "Spline")
);

}

namespace wm
{

void regist_rttr()
{
}

}