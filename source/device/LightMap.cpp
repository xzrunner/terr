#include "terraingraph/device/LightMap.h"
#include "terraingraph/DeviceHelper.h"
#include "terraingraph/HemanHelper.h"
#include "terraingraph/Bitmap.h"
#include "terraingraph/TextureGen.h"
#include "terraingraph/Utility.h"

#include <heightfield/HeightField.h>

#include <heman.h>

namespace terraingraph
{
namespace device
{

void LightMap::Execute(const std::shared_ptr<dag::Context>& ctx)
{
    auto prev_hf = DeviceHelper::GetInputHeight(*this, 0);
    if (!prev_hf) {
        return;
    }

    size_t w = prev_hf->Width();
    size_t h = prev_hf->Height();

    std::vector<int32_t> heights = prev_hf->GetValues();

    const float scale[] = { 1, 1, 1 };
    uint8_t* normals = TextureGen::CalcNormals(
        heights.data(), w, h, scale, 0.1f, 0.75f, 8
    );
    uint8_t* shadows = TextureGen::CalcShadows(
        heights.data(), w, h, scale, m_light_dir.xyz
    );
    uint8_t* lights = TextureGen::CalcLighting(
        normals, shadows, w, h, scale, m_ambient, m_diffuse, m_light_dir.xyz
    );

    m_bmp = std::make_shared<Bitmap>(w, h);

    std::vector<unsigned char> light_data(w * h * 3);
    for (size_t i = 0, n = w * h; i < n; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            light_data[i * 3 + j] = lights[i];
        }
    }
    delete[] normals;
    delete[] shadows;
    delete[] lights;

    m_bmp->SetValues(light_data);
}

}
}