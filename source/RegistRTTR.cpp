// generator
#include "wm/device/BasicNoise.h"
#include "wm/device/Color.h"
#include "wm/device/FaultFractal.h"
#include "wm/device/FileInput.h"
#include "wm/device/LayoutGen.h"
#include "wm/device/PerlinNoise.h"
#include "wm/device/PlasmaFractal.h"
// output
#include "wm/device/OverlayView.h"
// combiner
#include "wm/device/Chooser.h"
#include "wm/device/Combiner.h"
// filter
#include "wm/device/Curves.h"
// natural
#include "wm/device/Erosion.h"
// selector
#include "wm/device/SelectHeight.h"
#include "wm/device/SelectSlope.h"
// converter
#include "wm/device/Colorizer.h"

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

// base

rttr::registration::class_<dag::Node<wm::DeviceVarType>::Port>("wm::Operation::Port")
	.property("var", &dag::Node<wm::DeviceVarType>::Port::var)
;

rttr::registration::class_<wm::Device>("wm::Device")
	.method("GetImports", &wm::Device::GetImports)
	.method("GetExports", &wm::Device::GetExports)
;

// generator

rttr::registration::class_<wm::device::BasicNoise>("wm::basic_noise")
.constructor<>()
#define PARM_FILEPATH "wm/device/BasicNoise.parm.h"
#define PARM_NODE_CLASS wm::device::BasicNoise
#include <dag/rttr_prop_gen.h>
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

rttr::registration::class_<wm::device::BasicNoise>("wm::basic_noise")
.constructor<>()
#define PARM_FILEPATH "wm/device/BasicNoise.parm.h"
#define PARM_NODE_CLASS wm::device::BasicNoise
#include <dag/rttr_prop_gen.h>
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

rttr::registration::class_<wm::device::Color>("wm::color")
.constructor<>()
#define PARM_FILEPATH "wm/device/Color.parm.h"
#define PARM_NODE_CLASS wm::device::Color
#include <dag/rttr_prop_gen.h>
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

rttr::registration::class_<wm::device::FaultFractal>("wm::fault_fractal")
.constructor<>()
#define PARM_FILEPATH "wm/device/FaultFractal.parm.h"
#define PARM_NODE_CLASS wm::device::FaultFractal
#include <dag/rttr_prop_gen.h>
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

rttr::registration::class_<wm::device::FileInput>("wm::file_input")
.constructor<>()
#define PARM_FILEPATH "wm/device/FileInput.parm.h"
#define PARM_NODE_CLASS wm::device::FileInput
#include <dag/rttr_prop_gen.h>
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

rttr::registration::class_<wm::device::LayoutGen>("wm::layout_gen")
.constructor<>()
#define PARM_FILEPATH "wm/device/LayoutGen.parm.h"
#define PARM_NODE_CLASS wm::device::LayoutGen
#include <dag/rttr_prop_gen.h>
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

rttr::registration::class_<wm::device::PerlinNoise>("wm::perlin_noise")
.constructor<>()
#define PARM_FILEPATH "wm/device/PerlinNoise.parm.h"
#define PARM_NODE_CLASS wm::device::PerlinNoise
#include <dag/rttr_prop_gen.h>
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

rttr::registration::class_<wm::device::PlasmaFractal>("wm::plasma_fractal")
.constructor<>()
#define PARM_FILEPATH "wm/device/PlasmaFractal.parm.h"
#define PARM_NODE_CLASS wm::device::PlasmaFractal
#include <dag/rttr_prop_gen.h>
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

// output

rttr::registration::class_<wm::device::OverlayView>("wm::overlay_view")
.constructor<>()
#define PARM_FILEPATH "wm/device/OverlayView.parm.h"
#define PARM_NODE_CLASS wm::device::OverlayView
#include <dag/rttr_prop_gen.h>
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

// combiner

rttr::registration::class_<wm::device::Chooser>("wm::chooser")
.constructor<>()
#define PARM_FILEPATH "wm/device/Chooser.parm.h"
#define PARM_NODE_CLASS wm::device::Chooser
#include <dag/rttr_prop_gen.h>
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

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
rttr::registration::class_<wm::device::Combiner>("wm::combiner")
.constructor<>()
#define PARM_FILEPATH "wm/device/Combiner.parm.h"
#define PARM_NODE_CLASS wm::device::Combiner
#include <dag/rttr_prop_gen.h>
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

// filter

rttr::registration::enumeration<wm::device::Curves::Type>("terr_curves_type")
(
    REGIST_ENUM_ITEM(wm::device::Curves::Type::Linear, "linear", "Linear"),
    REGIST_ENUM_ITEM(wm::device::Curves::Type::Spline, "spline", "Spline")
    );
rttr::registration::class_<wm::device::Curves>("wm::curves")
.constructor<>()
#define PARM_FILEPATH "wm/device/Curves.parm.h"
#define PARM_NODE_CLASS wm::device::Curves
#include <dag/rttr_prop_gen.h>
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

// natural

rttr::registration::class_<wm::device::Erosion>("wm::erosion")
.constructor<>()
#define PARM_FILEPATH "wm/device/Erosion.parm.h"
#define PARM_NODE_CLASS wm::device::Erosion
#include <dag/rttr_prop_gen.h>
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

// selector

rttr::registration::class_<wm::device::SelectHeight>("wm::select_height")
.constructor<>()
#define PARM_FILEPATH "wm/device/SelectHeight.parm.h"
#define PARM_NODE_CLASS wm::device::SelectHeight
#include <dag/rttr_prop_gen.h>
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

rttr::registration::class_<wm::device::SelectSlope>("wm::select_slope")
.constructor<>()
#define PARM_FILEPATH "wm/device/SelectSlope.parm.h"
#define PARM_NODE_CLASS wm::device::SelectSlope
#include <dag/rttr_prop_gen.h>
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

// converter

rttr::registration::class_<wm::device::Colorizer>("wm::colorizer")
.constructor<>()
#define PARM_FILEPATH "wm/device/Colorizer.parm.h"
#define PARM_NODE_CLASS wm::device::Colorizer
#include <dag/rttr_prop_gen.h>
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

}

namespace wm
{

void regist_rttr()
{
}

}