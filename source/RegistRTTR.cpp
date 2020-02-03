// generator
#include "terr/device/BasicNoise.h"
#include "terr/device/Color.h"
#include "terr/device/FaultFractal.h"
#include "terr/device/FileInput.h"
#include "terr/device/PerlinNoise.h"
#include "terr/device/PlasmaFractal.h"
// output
#include "terr/device/OverlayView.h"
// combiner
#include "terr/device/Chooser.h"
#include "terr/device/Combiner.h"
// filter
#include "terr/device/Curves.h"
// natural
#include "terr/device/Erosion.h"
// selector
#include "terr/device/SelectHeight.h"
#include "terr/device/SelectSlope.h"
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

rttr::registration::class_<terr::device::BasicNoise>("terr::basic_noise")
.constructor<>()
#define PARM_FILEPATH "terr/device/BasicNoise.parm.h"
#define PARM_NODE_CLASS terr::device::BasicNoise
#include <dag/rttr_prop_gen.h>
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

rttr::registration::class_<terr::device::Color>("terr::color")
.constructor<>()
#define PARM_FILEPATH "terr/device/Color.parm.h"
#define PARM_NODE_CLASS terr::device::Color
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

// output

rttr::registration::class_<terr::device::OverlayView>("terr::overlay_view")
.constructor<>()
#define PARM_FILEPATH "terr/device/OverlayView.parm.h"
#define PARM_NODE_CLASS terr::device::OverlayView
#include <dag/rttr_prop_gen.h>
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

// combiner

rttr::registration::class_<terr::device::Chooser>("terr::chooser")
.constructor<>()
#define PARM_FILEPATH "terr/device/Chooser.parm.h"
#define PARM_NODE_CLASS terr::device::Chooser
#include <dag/rttr_prop_gen.h>
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

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

// filter

rttr::registration::enumeration<terr::device::Curves::Type>("terr_curves_type")
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

// natural

rttr::registration::class_<terr::device::Erosion>("terr::erosion")
.constructor<>()
#define PARM_FILEPATH "terr/device/Erosion.parm.h"
#define PARM_NODE_CLASS terr::device::Erosion
#include <dag/rttr_prop_gen.h>
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

// selector

rttr::registration::class_<terr::device::SelectHeight>("terr::select_height")
.constructor<>()
#define PARM_FILEPATH "terr/device/SelectHeight.parm.h"
#define PARM_NODE_CLASS terr::device::SelectHeight
#include <dag/rttr_prop_gen.h>
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

rttr::registration::class_<terr::device::SelectSlope>("terr::select_slope")
.constructor<>()
#define PARM_FILEPATH "terr/device/SelectSlope.parm.h"
#define PARM_NODE_CLASS terr::device::SelectSlope
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