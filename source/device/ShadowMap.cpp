#include "terraingraph/device/ShadowMap.h"
#include "terraingraph/DeviceHelper.h"
#include "terraingraph/HemanHelper.h"
#include "terraingraph/Bitmap.h"
#include "terraingraph/TextureGen.h"

#include <heightfield/HeightField.h>

#include <heman.h>

namespace terraingraph
{
namespace device
{

void ShadowMap::Execute(const std::shared_ptr<dag::Context>& ctx)
{
    auto prev_hf = DeviceHelper::GetInputHeight(*this, 0);
    if (!prev_hf) {
        return;
    }

    size_t w = prev_hf->Width();
    size_t h = prev_hf->Height();

    std::vector<int32_t> heights = prev_hf->GetValues();

    const float scale[] = { 1, 1, 1 };
    uint8_t* shadows = TextureGen::CalcShadows(
        heights.data(), w, h, scale, m_light_dir.xyz
    );

    m_bmp = std::make_shared<Bitmap>(w, h);

    std::vector<unsigned char> shadow_data(w * h * 3);
    for (size_t i = 0, n = w * h; i < n; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            shadow_data[i * 3 + j] = shadows[i];
        }
    }
    delete[] shadows;

    m_bmp->SetValues(shadow_data);
}

}
}