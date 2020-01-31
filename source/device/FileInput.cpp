#include "terr/device/FileInput.h"
#include "terr/HeightField.h"

#include <gimg_import.h>
#include <gimg_typedef.h>

#include <boost/filesystem.hpp>

namespace terr
{
namespace device
{

void FileInput::Execute(const Context& ctx)
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

            std::vector<float> height(sz, 0.0f);
            for (size_t i = 0; i < sz; ++i) {
                height[i] = static_cast<unsigned char>(pixels[i]) / 255.0f;
            }

            m_hf = std::make_shared<HeightField>(size, size);
            m_hf->Fill(height);
        }
        fin.close();
    }
    else
    {
	    int width, height, format;
	    uint8_t* pixels = gimg_import(m_filepath.c_str(), &width, &height, &format);
	    if (pixels)
	    {
            assert(format == GPF_ALPHA);

            size_t sz = width * height;
            std::vector<float> h_data(sz, 0.0f);
            for (size_t i = 0; i < sz; ++i) {
                h_data[i] = pixels[i] / 255.0f;
            }

            m_hf = std::make_shared<HeightField>(width, height);
            m_hf->Fill(h_data);
	    }
    }
}

}
}