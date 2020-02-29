#include "terraingraph/device/Color.h"
#include "terraingraph/Bitmap.h"

namespace terraingraph
{
namespace device
{

void Color::Execute()
{
    size_t w = 128;
    size_t h = 128;

    unsigned char rgb[3];
    for (size_t i = 0; i < 3; ++i) {
        rgb[i] = static_cast<unsigned char>(m_rgb.xyz[i]);
    }

    m_bmp = std::make_shared<Bitmap>(w, h);

    std::vector<unsigned char> pixels(w * h * 3);
    for (size_t i = 0, n = w * h; i < n; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            pixels[i * 3 + j] = rgb[j];
        }
    }
    m_bmp->SetValues(pixels);
}

}
}