#define EXE_FILEPATH "terraingraph/device_include_gen.h"
#include "terraingraph/device_regist_cfg.h"
#undef EXE_FILEPATH

#include <rttr/registration>

#define REGIST_NODE_TYPE(type, name)                            \
	rttr::registration::class_<terraingraph::device::type>("terraingraph::"#name) \
		.constructor<>()                                        \
	;

#define REGIST_ENUM_ITEM(type, name, label) \
    rttr::value(name, type),                \
    rttr::metadata(type, label)             \

RTTR_REGISTRATION
{

rttr::registration::class_<dag::Node<terraingraph::DeviceVarType>::Port>("terraingraph::Operation::Port")
	.property("var", &dag::Node<terraingraph::DeviceVarType>::Port::var)
;

rttr::registration::class_<terraingraph::Device>("terraingraph::Device")
	.method("GetImports", &terraingraph::Device::GetImports)
	.method("GetExports", &terraingraph::Device::GetExports)
;

#define EXE_FILEPATH "terraingraph/device_rttr_gen.h"
#include "terraingraph/device_regist_cfg.h"
#undef EXE_FILEPATH

rttr::registration::enumeration<terraingraph::NoiseQuality>("terr_noise_quality")
(
    REGIST_ENUM_ITEM(terraingraph::NoiseQuality::Fast,     "fast",     "Fast"),
    REGIST_ENUM_ITEM(terraingraph::NoiseQuality::Standard, "standard", "Standard"),
    REGIST_ENUM_ITEM(terraingraph::NoiseQuality::Best,      "best",    "Best")
);

rttr::registration::enumeration<terraingraph::device::TurbulenceNoise::Type>("terr_advance_noise_type")
(
    REGIST_ENUM_ITEM(terraingraph::device::TurbulenceNoise::Type::Perlin,            "perlin",               "Perlin"),
    REGIST_ENUM_ITEM(terraingraph::device::TurbulenceNoise::Type::Turbulence,        "turbulence",           "Turbulence"),
    REGIST_ENUM_ITEM(terraingraph::device::TurbulenceNoise::Type::BillowyTurbulence, "billowy_turbulence",   "Billowy Turbulence"),
    REGIST_ENUM_ITEM(terraingraph::device::TurbulenceNoise::Type::RidgedTurbulence,  "ridged_turbulence",    "Ridged Turbulence"),
    REGIST_ENUM_ITEM(terraingraph::device::TurbulenceNoise::Type::IqTurbulence,      "iq_turbulence",        "Iq Turbulence"),
    REGIST_ENUM_ITEM(terraingraph::device::TurbulenceNoise::Type::SwissTurbulence,   "swiss_turbulence",     "Swiss Turbulence"),
    REGIST_ENUM_ITEM(terraingraph::device::TurbulenceNoise::Type::JordanTurbulence,  "jordan_turbulence",    "Jordan Turbulence"),
    REGIST_ENUM_ITEM(terraingraph::device::TurbulenceNoise::Type::Erosion1Turbulence, "erosion1_turbulence", "Erosion1 Turbulence"),
    REGIST_ENUM_ITEM(terraingraph::device::TurbulenceNoise::Type::Erosion2Turbulence, "erosion2_turbulence", "Erosion2 Turbulence")
);

rttr::registration::enumeration<terraingraph::device::Combiner::Method> ("terr_combiner_method")
(
    REGIST_ENUM_ITEM(terraingraph::device::Combiner::Method::Average,  "average",  "Average"),
    REGIST_ENUM_ITEM(terraingraph::device::Combiner::Method::Add,      "add",      "Add"),
    REGIST_ENUM_ITEM(terraingraph::device::Combiner::Method::Subtract, "subtract", "Subtract"),
    REGIST_ENUM_ITEM(terraingraph::device::Combiner::Method::Multiply, "multiply", "Multiply"),
    REGIST_ENUM_ITEM(terraingraph::device::Combiner::Method::Divide,   "divide",   "Divide"),
    REGIST_ENUM_ITEM(terraingraph::device::Combiner::Method::Max,      "max",      "Max"),
    REGIST_ENUM_ITEM(terraingraph::device::Combiner::Method::Min,      "min",      "Min")
);

rttr::registration::enumeration<terraingraph::device::Curves::Type>("terr_curves_type")
(
    REGIST_ENUM_ITEM(terraingraph::device::Curves::Type::Linear, "linear", "Linear"),
    REGIST_ENUM_ITEM(terraingraph::device::Curves::Type::Spline, "spline", "Spline")
);

}

namespace terraingraph
{

void regist_rttr()
{
}

}