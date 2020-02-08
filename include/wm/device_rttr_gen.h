#define XSTR(s) STR(s)
#define STR(s) #s

#ifndef PARM_DEVICE_CLASS
#error "You must define PARM_DEVICE_CLASS macro before include this file"
#endif

#ifndef PARM_DEVICE_NAME
#error "You must define PARM_DEVICE_NAME macro before include this file"
#endif

#ifndef PARM_FILEPATH_PARM
#define PARM_FILEPATH_PARM wm/device/##PARM_DEVICE_CLASS##.parm.h
#endif

#define RTTR_NAME wm::##PARM_DEVICE_NAME

rttr::registration::class_<wm::device::PARM_DEVICE_CLASS>(XSTR(RTTR_NAME))
.constructor<>()
#define PARM_FILEPATH XSTR(PARM_FILEPATH_PARM)
#define PARM_NODE_CLASS wm::device::##PARM_DEVICE_CLASS
#include <dag/rttr_prop_gen.h>
#undef PARM_FILEPATH
#undef PARM_NODE_CLASS
;

#undef PARM_DEVICE_NAME
#undef PARM_DEVICE_CLASS
#undef PARM_FILEPATH_PARM
