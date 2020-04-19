#include "terraingraph/device/StreamPowerErosion.h"
#include "terraingraph/DeviceHelper.h"
#include "terraingraph/HeightFieldEval.h"
#include "terraingraph/Context.h"

#include <heightfield/HeightField.h>

namespace terraingraph
{
namespace device
{

// from Outerrain
/*
\brief Perform a stream power erosion step with maximum amplitude defined by user. Based on https://hal.inria.fr/hal-01262376/document.
This erosion called 'Fluvial' is based on Drainasge and Slope. One of the weakness of the stream power erosion is that it can create peaks
and generally unrealistic height sometimes. Therefore it can be improved by checking for slopes higher than 30� and not performing erosion.

\param amplitude maximum amount of matter eroded in one step. Something between [0.5, 1.0] gives plausible results.
*/
void StreamPowerErosion::Execute(const std::shared_ptr<dag::Context>& ctx)
{
    auto prev_hf = DeviceHelper::GetInputHeight(*this, 0);
    if (!prev_hf) {
        return;
    }

    m_hf = std::make_shared<hf::HeightField>(*prev_hf);

    auto& dev = *std::static_pointer_cast<Context>(ctx)->ur_dev;

    hf::ScalarField2D<float> SP = HeightFieldEval::StreamPower(dev, *m_hf);
    for (size_t y = 0, h = m_hf->Height(); y < h; ++y) {
        for (size_t x = 0, w = m_hf->Width(); x < w; ++x) {
            int32_t oldH = m_hf->Get(dev, x, y);
            int32_t newH = oldH - static_cast<int32_t>(SP.Get(x, y) * m_amplitude);
            m_hf->Set(x, y, newH);
        }
    }
}

}
}