#pragma once

#include <unirender/Texture.h>

#include <stdint.h>

namespace terr
{

class HeightMapTex
{
public:
	HeightMapTex(bool use_height_info = false);
	~HeightMapTex();

	void LoadFromRawFile(const char* filepath, size_t size);
	void LoadFromImgFile(const char* filepath);

	// gen
	void MakeHeightMapPlasma(int size, float roughness);
	void MakeHeightMapFault(int size, int iIterations, int iMinDelta, int iMaxDelta, float fFilter);

	void Bind(int channel) const;

	int GetHeight(int x, int z) const;

	const uint8_t* Data() const { return m_height_info; }
	int Size() const { return m_tex->Width(); }

private:
	// Scale the terrain height values to a range of 0-255
	static void NormalizeTerrain(int size, float* height_data);

	static void FilterHeightBand(float* fpBand, int iStride, int iCount, float fFilter);
	static void FilterHeightField(int sizef, float* fpHeightData, float fFilter);

private:
	bool m_use_height_info = false;
	uint8_t* m_height_info = nullptr;

	ur::TexturePtr m_tex = nullptr;

}; // HeightMapTex

}