#ifndef EXE_FILEPATH
#error "You must define EXE_FILEPATH macro before include this file"
#endif

// generator

#define PARM_DEVICE_CLASS Color
#define PARM_DEVICE_NAME color
#include EXE_FILEPATH

#define PARM_DEVICE_CLASS Constant
#define PARM_DEVICE_NAME constant
#include EXE_FILEPATH

#define PARM_DEVICE_CLASS Gradient
#define PARM_DEVICE_NAME gradient
#include EXE_FILEPATH

#define PARM_DEVICE_CLASS NoiseBrush
#define PARM_DEVICE_NAME noise_brush
#include EXE_FILEPATH

#define PARM_DEVICE_CLASS TemplateBrush
#define PARM_DEVICE_NAME template_brush
#include EXE_FILEPATH

#define PARM_DEVICE_CLASS AutoGen
#define PARM_DEVICE_NAME auto_gen
#include EXE_FILEPATH

#define PARM_DEVICE_CLASS FileInput
#define PARM_DEVICE_NAME file_input
#include EXE_FILEPATH

#define PARM_DEVICE_CLASS LayoutGen
#define PARM_DEVICE_NAME layout_gen
#include EXE_FILEPATH

#define PARM_DEVICE_CLASS FaultFractal
#define PARM_DEVICE_NAME fault_fractal
#include EXE_FILEPATH

#define PARM_DEVICE_CLASS PlasmaFractal
#define PARM_DEVICE_NAME plasma_fractal
#include EXE_FILEPATH

#define PARM_DEVICE_CLASS BasicNoise
#define PARM_DEVICE_NAME basic_noise
#include EXE_FILEPATH

#define PARM_DEVICE_CLASS Billow
#define PARM_DEVICE_NAME billow
#include EXE_FILEPATH

#define PARM_DEVICE_CLASS CellularNoise
#define PARM_DEVICE_NAME cellular_noise
#include EXE_FILEPATH

#define PARM_DEVICE_CLASS FBM
#define PARM_DEVICE_NAME fbm
#include EXE_FILEPATH

#define PARM_DEVICE_CLASS PerlinNoise
#define PARM_DEVICE_NAME perlin_noise
#include EXE_FILEPATH

#define PARM_DEVICE_CLASS PerlinNoise2
#define PARM_DEVICE_NAME perlin_noise2
#include EXE_FILEPATH

#define PARM_DEVICE_CLASS RidgedMulti
#define PARM_DEVICE_NAME ridged_multi
#include EXE_FILEPATH

#define PARM_DEVICE_CLASS TurbulenceNoise
#define PARM_DEVICE_NAME turbulence_noise
#include EXE_FILEPATH

#define PARM_DEVICE_CLASS TurbulenceNoise2
#define PARM_DEVICE_NAME turbulence_noise2
#include EXE_FILEPATH

#define PARM_DEVICE_CLASS Voronoi
#define PARM_DEVICE_NAME voronoi
#include EXE_FILEPATH

#define PARM_DEVICE_CLASS Archipelago
#define PARM_DEVICE_NAME archipelago
#include EXE_FILEPATH

#define PARM_DEVICE_CLASS Island
#define PARM_DEVICE_NAME island
#include EXE_FILEPATH

#define PARM_DEVICE_CLASS Planet
#define PARM_DEVICE_NAME planet
#include EXE_FILEPATH

#define PARM_DEVICE_CLASS Rock
#define PARM_DEVICE_NAME rock
#include EXE_FILEPATH

// output

#define PARM_DEVICE_CLASS HeightOutput
#define PARM_DEVICE_NAME height_output
#include EXE_FILEPATH

#define PARM_DEVICE_CLASS OverlayView
#define PARM_DEVICE_NAME overlay_view
#include EXE_FILEPATH

// combiner

#define PARM_DEVICE_CLASS Chooser
#define PARM_DEVICE_NAME chooser
#include EXE_FILEPATH

#define PARM_DEVICE_CLASS Combiner
#define PARM_DEVICE_NAME combiner
#include EXE_FILEPATH

// filter

#define PARM_DEVICE_CLASS Clamp
#define PARM_DEVICE_NAME clamp
#include EXE_FILEPATH

#define PARM_DEVICE_CLASS Curves
#define PARM_DEVICE_NAME curves
#include EXE_FILEPATH

#define PARM_DEVICE_CLASS Gaussian
#define PARM_DEVICE_NAME gaussian
#include EXE_FILEPATH

#define PARM_DEVICE_CLASS Incline
#define PARM_DEVICE_NAME incline
#include EXE_FILEPATH

#define PARM_DEVICE_CLASS Inverter
#define PARM_DEVICE_NAME inverter
#include EXE_FILEPATH

#define PARM_DEVICE_CLASS ScaleBias
#define PARM_DEVICE_NAME scale_bias
#include EXE_FILEPATH

#define PARM_DEVICE_CLASS Step
#define PARM_DEVICE_NAME step
#include EXE_FILEPATH

#define PARM_DEVICE_CLASS Transform
#define PARM_DEVICE_NAME transform
#include EXE_FILEPATH

// natural

#define PARM_DEVICE_CLASS Erosion
#define PARM_DEVICE_NAME erosion
#include EXE_FILEPATH

#define PARM_DEVICE_CLASS HydraulicErosion
#define PARM_DEVICE_NAME hydraulic_erosion
#include EXE_FILEPATH

#define PARM_DEVICE_CLASS StreamPowerErosion
#define PARM_DEVICE_NAME stream_power_erosion
#include EXE_FILEPATH

#define PARM_DEVICE_CLASS ThermalWeathering
#define PARM_DEVICE_NAME thermal_weathering
#include EXE_FILEPATH

#define PARM_DEVICE_CLASS Wind
#define PARM_DEVICE_NAME wind
#include EXE_FILEPATH

// selector

#define PARM_DEVICE_CLASS SelectHeight
#define PARM_DEVICE_NAME select_height
#include EXE_FILEPATH

#define PARM_DEVICE_CLASS SelectMask
#define PARM_DEVICE_NAME select_mask
#include EXE_FILEPATH

#define PARM_DEVICE_CLASS SelectSlope
#define PARM_DEVICE_NAME select_slope
#include EXE_FILEPATH

// converter

#define PARM_DEVICE_CLASS AlbedoMap
#define PARM_DEVICE_NAME albedo_map
#include EXE_FILEPATH

#define PARM_DEVICE_CLASS AmbientOcclusion
#define PARM_DEVICE_NAME ambient_occlusion
#include EXE_FILEPATH

#define PARM_DEVICE_CLASS LightMap
#define PARM_DEVICE_NAME light_map
#include EXE_FILEPATH

#define PARM_DEVICE_CLASS NormalMap
#define PARM_DEVICE_NAME normal_map
#include EXE_FILEPATH

#define PARM_DEVICE_CLASS ShadowMap
#define PARM_DEVICE_NAME shadow_map
#include EXE_FILEPATH

#define PARM_DEVICE_CLASS Colorizer
#define PARM_DEVICE_NAME colorizer
#include EXE_FILEPATH

#define PARM_DEVICE_CLASS Lighting
#define PARM_DEVICE_NAME lighting
#include EXE_FILEPATH

#define PARM_DEVICE_CLASS Resample
#define PARM_DEVICE_NAME resample
#include EXE_FILEPATH