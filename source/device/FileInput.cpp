#include "terraingraph/device/FileInput.h"
#include "terraingraph/Mask.h"

#include <heightfield/Loader.h>
#include <heightfield/HeightField.h>

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

    bool is_mask = true;
    for (auto& v : hf->GetValues()) {
        if (v > 0.0f && v < 1.0f) {
            is_mask = false;
            break;
        }
    }
    if (!hf->GetShortValues().empty()) {
        is_mask = false;
    }

    if (is_mask)
    {
        m_mask = std::make_shared<Mask>(hf->Width(), hf->Height());

        auto& src = hf->GetValues();
        std::vector<bool> dst(src.size());
        for (size_t i = 0, n = src.size(); i < n; ++i) {
            dst[i] = src[i] == 0 ? false : true;
        }
        m_mask->SetValues(dst);
    }
    else
    {
        m_hf = hf;
    }
}

}
}