#include "terraingraph/device/FileInput.h"
#include "terraingraph/HeightField.h"
#include "terraingraph/Mask.h"

#include <gimg_import.h>
#include <gimg_typedef.h>

#include <boost/filesystem.hpp>

namespace terraingraph
{
namespace device
{

void FileInput::Execute()
{
    if (m_filepath.empty()) {
        return;
    }

    auto ext = boost::filesystem::extension(m_filepath);
    std::transform(ext.begin(), ext.end(), ext.begin(), tolower);
    if (ext == ".raw")
    {
        std::ifstream fin(m_filepath, std::ios::binary | std::ios::ate);
        if (fin.fail()) {
            return;
        }

        size_t sz = static_cast<size_t>(fin.tellg());
        fin.seekg(0, std::ios::beg);

        std::vector<char> pixels(sz, 0);
        if (fin.read(pixels.data(), sz))
        {
            size_t size = static_cast<size_t>(std::sqrt(sz));
            assert(size * size == sz);

            bool is_mask = true;
            for (auto& p : pixels) {
                if (p != 0 && p != 255) {
                    is_mask = false;
                    break;
                }
            }

            if (is_mask)
            {
                std::vector<bool> mask(sz, false);
                for (size_t i = 0; i < sz; ++i) {
                    mask[i] = pixels[i] == 0 ? false : true;
                }

                m_mask = std::make_shared<Mask>(size, size);
                m_mask->SetValues(mask);
            }
            else
            {
                std::vector<float> height(sz, 0.0f);
                for (size_t i = 0; i < sz; ++i) {
                    height[i] = static_cast<unsigned char>(pixels[i]) / 255.0f;
                }

                m_hf = std::make_shared<HeightField>(size, size);
                m_hf->SetValues(height);
            }
        }
        fin.close();
    }
    else
    {
	    int width, height, format;
	    uint8_t* pixels = gimg_import(m_filepath.c_str(), &width, &height, &format);
        if (!pixels) {
            return;
        }

        bool is_mask = true;
        switch (format)
        {
        case GPF_ALPHA:
        case GPF_LUMINANCE:
        case GPF_LUMINANCE_ALPHA:
            for (size_t i = 0, n = width * height; i < n; ++i) {
                if (pixels[i] != 0 && pixels[i] != 255) {
                    is_mask = false;
                    break;
                }
            }
            break;
        case GPF_RGB:
            for (size_t i = 0, n = width * height; i < n && is_mask; ++i)
            {
                for (size_t j = 0; j < 3 && is_mask; ++j) {
                    size_t idx = i * 3 + j;
                    if (pixels[idx] != 0 && pixels[idx] != 255) {
                        is_mask = false;
                        break;
                    }
                }
                for (size_t j = 1; j < 3 && is_mask; ++j) {
                    if (pixels[i * 3] != pixels[i * 3 + j]) {
                        is_mask = false;
                        break;
                    }
                }
            }
            break;
        case GPF_RGBA8:
            for (size_t i = 0, n = width * height; i < n && is_mask; ++i)
            {
                for (size_t j = 0; j < 4 && is_mask; ++j) {
                    size_t idx = i * 4 + j;
                    if (pixels[idx] != 0 && pixels[idx] != 255) {
                        is_mask = false;
                        break;
                    }
                }
                for (size_t j = 1; j < 3 && is_mask; ++j) {
                    if (pixels[i * 4] != pixels[i * 4 + j]) {
                        is_mask = false;
                        break;
                    }
                }
            }
            break;
        default:
            assert(0);
        }

        size_t sz = width * height;
        if (is_mask)
        {
            std::vector<bool> mask(sz, false);
            switch (format)
            {
            case GPF_ALPHA:
            case GPF_LUMINANCE:
            case GPF_LUMINANCE_ALPHA:
                for (size_t i = 0; i < sz; ++i) {
                    mask[i] = pixels[i] == 0 ? false : true;
                }
                break;
            case GPF_RGB:
                for (size_t i = 0; i < sz; ++i) {
                    mask[i] = pixels[i * 3] == 0 ? false : true;
                }
                break;
            case GPF_RGBA8:
                for (size_t i = 0; i < sz; ++i) {
                    mask[i] = pixels[i * 4] == 0 ? false : true;
                }
                break;
            default:
                assert(0);
            }

            m_mask = std::make_shared<Mask>(width, height);
            m_mask->SetValues(mask);
        }
        else
        {
            std::vector<float> h_data(sz, 0.0f);
            switch (format)
            {
            case GPF_ALPHA:
            case GPF_LUMINANCE:
            case GPF_LUMINANCE_ALPHA:
                for (size_t i = 0; i < sz; ++i) {
                    h_data[i] = pixels[i] / 255.0f;
                }
                break;
            case GPF_RGB:
                for (size_t i = 0; i < sz; ++i) {
                    h_data[i] = pixels[i * 3] / 255.0f;
                }
                break;
            case GPF_RGBA8:
                for (size_t i = 0; i < sz; ++i) {
                    h_data[i] = pixels[i * 4] / 255.0f;
                }
                break;
            default:
                assert(0);
            }

            m_hf = std::make_shared<HeightField>(width, height);
            m_hf->SetValues(h_data);
        }
    }
}

std::shared_ptr<HeightField>
FileInput::LoadHeightField(const std::string& filepath)
{
    std::shared_ptr<HeightField> hf = nullptr;

    if (filepath.empty()) {
        return nullptr;
    }

    auto ext = boost::filesystem::extension(filepath);
    std::transform(ext.begin(), ext.end(), ext.begin(), tolower);
    if (ext == ".raw")
    {
        std::ifstream fin(filepath, std::ios::binary | std::ios::ate);
        if (fin.fail()) {
            return nullptr;
        }

        size_t sz = static_cast<size_t>(fin.tellg());
        fin.seekg(0, std::ios::beg);

        std::vector<char> pixels(sz, 0);
        if (fin.read(pixels.data(), sz))
        {
            size_t size = static_cast<size_t>(std::sqrt(sz));
            assert(size * size == sz);

            std::vector<float> height(sz, 0.0f);
            for (size_t i = 0; i < sz; ++i) {
                height[i] = static_cast<unsigned char>(pixels[i]) / 255.0f;
            }

            hf = std::make_shared<HeightField>(size, size);
            hf->SetValues(height);
        }
        fin.close();
    }
    else
    {
	    int width, height, format;
	    uint8_t* pixels = gimg_import(filepath.c_str(), &width, &height, &format);
        if (!pixels) {
            return nullptr;
        }

        size_t sz = width * height;
        std::vector<float> h_data(sz, 0.0f);
        switch (format)
        {
        case GPF_ALPHA:
        case GPF_LUMINANCE:
        case GPF_LUMINANCE_ALPHA:
            for (size_t i = 0; i < sz; ++i) {
                h_data[i] = pixels[i] / 255.0f;
            }
            break;
        case GPF_RGB:
            for (size_t i = 0; i < sz; ++i) {
                h_data[i] = pixels[i * 3] / 255.0f;
            }
            break;
        case GPF_RGBA8:
            for (size_t i = 0; i < sz; ++i) {
                h_data[i] = pixels[i * 4] / 255.0f;
            }
            break;
        default:
            assert(0);
        }

        hf = std::make_shared<HeightField>(width, height);
        hf->SetValues(h_data);
    }

    return hf;
}

}
}