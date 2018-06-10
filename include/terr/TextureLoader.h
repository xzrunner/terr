#pragma once

#include "terr/Texture.h"

namespace terr
{

class TextureLoader
{
public:
	static TexturePtr LoadFromRawFile(const char* filepath, size_t size, int bpp);

}; // TextureLoader

}