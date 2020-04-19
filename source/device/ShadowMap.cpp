#include "terraingraph/device/ShadowMap.h"
#include "terraingraph/DeviceHelper.h"
#include "terraingraph/HemanHelper.h"
#include "terraingraph/TextureGen.h"
#include "terraingraph/Context.h"

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

    auto& dev = *std::static_pointer_cast<Context>(ctx)->ur_dev;
    std::vector<int32_t> heights = prev_hf->GetValues(dev);

    const float scale[] = { 1, 1, 1 };
    uint8_t* shadows = TextureGen::CalcShadows(
        heights.data(), w, h, scale, m_light_dir.xyz
    );

    m_bmp = std::make_shared<Bitmap>(w, h);
    auto shadow_data = m_bmp->GetPixels();
    for (size_t i = 0, n = w * h; i < n; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            shadow_data[i * 3 + j] = shadows[i];
        }
    }
    delete[] shadows;
}

}
}