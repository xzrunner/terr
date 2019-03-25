#include "terr/TextureLoader.h"

#include <guard/check.h>
#include <unirender/Blackboard.h>
#include <unirender/RenderContext.h>
#include <gimg_import.h>
#include <gimg_typedef.h>

#include <boost/filesystem.hpp>

#include <fstream>

#include <assert.h>

namespace terr
{

ur::TexturePtr TextureLoader::LoadFromFile(const char* filepath)
{
	if (!boost::filesystem::is_regular_file(filepath)) {
		return false;
	}

	int w, h, fmt;
	uint8_t* pixels = gimg_import(filepath, &w, &h, &fmt);
	if (!pixels) {
		return false;
	}

	//if (fmt == GPF_RGBA8 && gum::Config::Instance()->GetPreMulAlpha()) {
	//	gimg_pre_mul_alpha(pixels, w, h);
	//}

	ur::TEXTURE_FORMAT tf = ur::TEXTURE_INVALID;
	switch (fmt)
	{
	case GPF_ALPHA: case GPF_LUMINANCE: case GPF_LUMINANCE_ALPHA:
		tf = ur::TEXTURE_A8;
		break;
	case GPF_RGB:
		tf = ur::TEXTURE_RGB;
		break;
	case GPF_RGBA8:
		tf = ur::TEXTURE_RGBA8;
		break;
	case GPF_BGRA_EXT:
		tf = ur::TEXTURE_BGRA_EXT;
		break;
	case GPF_BGR_EXT:
		tf = ur::TEXTURE_BGR_EXT;
		break;
    case GPF_RGB16F:
        tf = ur::TEXTURE_RGB16F;
        break;
    case GPF_RGB32F:
        tf = ur::TEXTURE_RGB32F;
        break;
	case GPF_COMPRESSED_RGBA_S3TC_DXT1_EXT:
		tf = ur::TEXTURE_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		break;
	case GPF_COMPRESSED_RGBA_S3TC_DXT3_EXT:
		tf = ur::TEXTURE_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		break;
	case GPF_COMPRESSED_RGBA_S3TC_DXT5_EXT:
		tf = ur::TEXTURE_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		break;
	default:
		GD_REPORT_ASSERT("unknown type.");
	}

	auto& rc = ur::Blackboard::Instance()->GetRenderContext();
	auto texid = rc.CreateTexture(pixels, w, h, tf);
	free(pixels);

	return std::make_unique<ur::Texture>(&rc, w, h, tf, texid);
}

ur::TexturePtr TextureLoader::LoadFromRawFile(const char* filepath, size_t size, int bpp)
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

		fin.close();

		auto& rc = ur::Blackboard::Instance()->GetRenderContext();
		int texid = rc.CreateTexture(pixels, size, size, format);
		return std::make_unique<ur::Texture>(&rc, size, size, format, texid);
	}
	fin.close();

	return nullptr;
}

}