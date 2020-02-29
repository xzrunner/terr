#define XSTR(s) STR(s)
#define STR(s) #s

#ifndef PARM_DEVICE_CLASS
#error "You must define PARM_DEVICE_CLASS macro before include this file"
#endif

#ifndef PARM_FILEPATH_H
#define PARM_FILEPATH_H terraingraph/device/##PARM_DEVICE_CLASS##.h
#endif

#include XSTR(PARM_FILEPATH_H)

#undef PARM_DEVICE_NAME
#undef PARM_DEVICE_CLASS
#undef PARM_FILEPATH_PARM
