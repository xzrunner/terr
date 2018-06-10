#pragma once

#include <cstdint>

namespace terr
{

class TextureGen
{
public:
	static void LightMapPut(uint8_t* light_map, int size, int x, int y, uint8_t val);
	static uint8_t LightMapGet(const uint8_t* light_map, int size, int x, int y);
	static const uint8_t* NormalMapGet(const uint8_t* normal_map, int size, int x, int y);

	static uint8_t* CalcNormals(const uint8_t* height_map, int size, const float scale[3],
		float rand_scale, float cutoff, int frequency);

	static uint8_t* CalcShadows(const uint8_t* height_map, int size,
		const float scale[3], const float dir[3]);

	static uint8_t* CalcLighting(const uint8_t* normal_map, const uint8_t* shadow_map,
		int size, const float scale[3], float amb, float diff, const float dir[3]);

}; // TextureGen

}