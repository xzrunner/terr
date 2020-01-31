// generator
#include "terr/device/BasicNoise.h"
#include "terr/device/FaultFractal.h"
#include "terr/device/FileInput.h"
#include "terr/device/PerlinNoise.h"
#include "terr/device/PlasmaFractal.h"
// natural
#include "terr/device/Erosion.h"

#include <rttr/registration>

#define REGIST_NODE_TYPE(type, name)                            \
	rttr::registration::class_<terr::device::type>("terr::"#name) \
		.constructor<>()                                        \
	;

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

// natural

rttr::registration::class_<terr::device::Erosion>("terr::erosion")
.constructor<>()
#define PARM_FILEPATH "terr/device/Erosion.parm.h"
#define PARM_NODE_CLASS terr::device::Erosion
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