#pragma once

#include <unirender/Texture.h>

namespace terr
{

class TextureLoader
{
public:
	static ur::TexturePtr LoadFromFile(const char* filepath);
	static ur::TexturePtr LoadFromRawFile(const char* filepath, size_t size, int bpp);

}; // TextureLoader

}