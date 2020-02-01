// generator
#include "terr/device/BasicNoise.h"
#include "terr/device/FaultFractal.h"
#include "terr/device/FileInput.h"
#include "terr/device/PerlinNoise.h"
#include "terr/device/PlasmaFractal.h"
// combiner
#include "terr/device/Combiner.h"
// filter
#include "terr/device/Curves.h"
// natural
#include "terr/device/Erosion.h"
// converter
#include "terr/device/Colorizer.h"

#include <rttr/registration>

#define REGIST_NODE_TYPE(type, name)                            \
	rttr::registration::class_<terr::device::type>("terr::"#name) \
		.constructor<>()                                        \
	;

#define REGIST_ENUM_ITEM(type, name, label) \
    rttr::value(name, type),                \
    rttr::metadata(type, label)             \

RTTR_REGISTRATION
{

// base

rttr::registration::class_<dag::Node<terr::DeviceVarType>::Port>("terr::Operation::Port")
	.property("var", &dag::Node<terr::DeviceVarType>::Port::var)
;

rttr::registration::class_<terr::Device>("terr::Device")
	.method("GetImports", &terr::Device::GetImports)
	.method("GetExports", &terr::Device::GetExports)
;

// generator

rttr::registration::class_<terr::device::BasicNoise>("terr::basic_noise")
.constructor<>()
#define PARM_FILEPATH "terr/device/BasicNoise.parm.h"
#define PARM_NODE_CLASS terr::device::BasicNoise
#include <dag/rttr_prop_gen.h>
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

rttr::registration::class_<terr::device::FaultFractal>("terr::fault_fractal")
.constructor<>()
#define PARM_FILEPATH "terr/device/FaultFractal.parm.h"
#define PARM_NODE_CLASS terr::device::FaultFractal
#include <dag/rttr_prop_gen.h>
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

rttr::registration::class_<terr::device::FileInput>("terr::file_input")
.constructor<>()
#define PARM_FILEPATH "terr/device/FileInput.parm.h"
#define PARM_NODE_CLASS terr::device::FileInput
#include <dag/rttr_prop_gen.h>
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

rttr::registration::class_<terr::device::PerlinNoise>("terr::perlin_noise")
.constructor<>()
#define PARM_FILEPATH "terr/device/PerlinNoise.parm.h"
#define PARM_NODE_CLASS terr::device::PerlinNoise
#include <dag/rttr_prop_gen.h>
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

rttr::registration::class_<terr::device::PlasmaFractal>("terr::plasma_fractal")
.constructor<>()
#define PARM_FILEPATH "terr/device/PlasmaFractal.parm.h"
#define PARM_NODE_CLASS terr::device::PlasmaFractal
#include <dag/rttr_prop_gen.h>
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

// filter

rttr::registration::enumeration<terr::device::Curves::Type> ("terr_curves_type")
(
    REGIST_ENUM_ITEM(terr::device::Curves::Type::Linear, "linear", "Linear"),
    REGIST_ENUM_ITEM(terr::device::Curves::Type::Spline, "spline", "Spline")
);
rttr::registration::class_<terr::device::Curves>("terr::curves")
.constructor<>()
#define PARM_FILEPATH "terr/device/Curves.parm.h"
#define PARM_NODE_CLASS terr::device::Curves
#include <dag/rttr_prop_gen.h>
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

// combiner

rttr::registration::enumeration<terr::device::Combiner::Method> ("terr_combiner_method")
(
    REGIST_ENUM_ITEM(terr::device::Combiner::Method::Average,  "average",  "Average"),
    REGIST_ENUM_ITEM(terr::device::Combiner::Method::Add,      "add",      "Add"),
    REGIST_ENUM_ITEM(terr::device::Combiner::Method::Subtract, "subtract", "Subtract"),
    REGIST_ENUM_ITEM(terr::device::Combiner::Method::Multiply, "multiply", "Multiply"),
    REGIST_ENUM_ITEM(terr::device::Combiner::Method::Divide,   "divide",   "Divide"),
    REGIST_ENUM_ITEM(terr::device::Combiner::Method::Max,      "max",      "Max"),
    REGIST_ENUM_ITEM(terr::device::Combiner::Method::Min,      "min",      "Min")
);
rttr::registration::class_<terr::device::Combiner>("terr::combiner")
.constructor<>()
#define PARM_FILEPATH "terr/device/Combiner.parm.h"
#define PARM_NODE_CLASS terr::device::Combiner
#include <dag/rttr_prop_gen.h>
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

// natural

rttr::registration::class_<terr::device::Erosion>("terr::erosion")
.constructor<>()
#define PARM_FILEPATH "terr/device/Erosion.parm.h"
#define PARM_NODE_CLASS terr::device::Erosion
#include <dag/rttr_prop_gen.h>
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

// converter

rttr::registration::class_<terr::device::Colorizer>("terr::colorizer")
.constructor<>()
#define PARM_FILEPATH "terr/device/Colorizer.parm.h"
#define PARM_NODE_CLASS terr::device::Colorizer
#include <dag/rttr_prop_gen.h>
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

}

namespace terr
{

void regist_rttr()
{
}

}