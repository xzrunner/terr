#include "terraingraph/device/SelectSlope.h"
#include "terraingraph/DeviceHelper.h"
#include "terraingraph/Bitmap.h"
#include "terraingraph/TextureGen.h"
#include "terraingraph/TextureBaker.h"

#include <SM_Calc.h>
#include <heightfield/HeightField.h>
#include <heightfield/Utility.h>

namespace terraingraph
{
namespace device
{

void SelectSlope::Execute(const std::shared_ptr<dag::Context>& ctx)
{
    auto prev_hf = DeviceHelper::GetInputHeight(*this, 0);
    if (!prev_hf) {
        return;
    }

    auto w = prev_hf->Width();
    auto h = prev_hf->Height();
    m_hf = std::make_shared<hf::HeightField>(w, h);
    std::vector<int32_t> vals(w * h);

    assert(w == h);
    float scale[] = { 1, 1, 1 };
    auto normals = TextureGen::CalcNormals(prev_hf->GetValues().data(), w, h, scale, 0, 0, 0);

    size_t ptr = 0;
    for (size_t i = 0, n = prev_hf->GetValues().size(); i < n; ++i)
    {
        sm::vec3 norm;
        norm.x = normals[ptr++] / 255.0f;
        norm.z = normals[ptr++] / 255.0f;
        norm.y = normals[ptr++] / 255.0f;
        float angle = sm::get_angle(sm::vec3(0, 0, 0), norm, sm::vec3(0, 1, 0));
        assert(angle > -SM_LARGE_EPSILON && angle < SM_PI * 0.5f + SM_LARGE_EPSILON);
        angle = std::max(0.0f, std::min(SM_PI * 0.5f, angle));
        float slope = angle / (SM_PI * 0.5f);
        if (slope >= m_min && slope <= m_max) {
            vals[i] = hf::Utility::HeightFloatToShort(1.0f);
        } else {
            vals[i] = 0;
        }
    }

    delete[] normals;

    m_hf->SetValues(vals);
}

}
}