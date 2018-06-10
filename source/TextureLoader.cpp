#include "terr/TextureLoader.h"

#include <unirender/Blackboard.h>
#include <unirender/RenderContext.h>

#include <fstream>
#include <assert.h>

namespace terr
{

TexturePtr TextureLoader::LoadFromRawFile(const char* filepath, size_t size, int bpp)
{
	std::ifstream fin(filepath, std::ios::binary | std::ios::ate);
	if (fin.fail()) {
		return nullptr;
	}

	size_t sz = static_cast<size_t>(fin.tellg());
	fin.seekg(0, std::ios::beg);

	assert(sz == size * size * bpp);
	auto pixels = new char[size * size * bpp];
	if (fin.read(pixels, size * size * bpp))
	{
		ur::TEXTURE_FORMAT format;
		switch (bpp)
		{
		case 1:
			format = ur::TEXTURE_A8;
			break;
		case 3:
			format = ur::TEXTURE_RGB;
			break;
		case 4:
			format = ur::TEXTURE_RGBA8;
			break;
		default:
			fin.close();
			return nullptr;
		}

		auto& rc = ur::Blackboard::Instance()->GetRenderContext();
		int texid = rc.CreateTexture(pixels, size, size, format);
		return std::make_unique<Texture>(texid, size, size, format);
	}
	fin.close();

	return nullptr;
}

}