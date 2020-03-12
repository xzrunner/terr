#include "terraingraph/device/FileInput.h"
#include "terraingraph/Mask.h"

#include <heightfield/Loader.h>
#include <heightfield/HeightField.h>
#include <heightfield/Utility.h>

namespace terraingraph
{
namespace device
{

void FileInput::Execute(const std::shared_ptr<dag::Context>& ctx)
{
    auto hf = hf::Loader::Load(m_filepath);
    if (!hf) {
        return;
    }

    const auto min = hf::Utility::HeightFloatToShort(-1.0f);
    const auto max = hf::Utility::HeightFloatToShort(1.0f);

    bool is_mask = true;
    for (auto& v : hf->GetValues()) 
    {
        if (v != min && v != max) {
            is_mask = false;
            break;
        }
    }

    if (is_mask)
    {
        m_mask = std::make_shared<Mask>(hf->Width(), hf->Height());
        auto dst = m_mask->GetPixels();
        auto& src = hf->GetValues();
        for (size_t i = 0, n = src.size(); i < n; ++i) {
            dst[i] = src[i] == min ? false : true;
        }
    }
    else
    {
        m_hf = hf;
    }
}

}
}