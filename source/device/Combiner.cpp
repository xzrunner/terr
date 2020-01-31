#include "terr/device/Combiner.h"
#include "terr/DeviceHelper.h"
#include "terr/HeightField.h"

namespace terr
{
namespace device
{

void Combiner::Execute(const Context& ctx)
{
    auto hf0 = DeviceHelper::GetInputHeight(*this, 0);
    auto hf1 = DeviceHelper::GetInputHeight(*this, 1);
    if (!hf0 || !hf1) {
        return;
    }

    if (hf0->Width() != hf1->Width() ||
        hf0->Height() != hf1->Height()) {
        return;
    }

    size_t w = hf0->Width();
    size_t h = hf0->Height();
    size_t sz = w * h;

    auto& v0 = hf0->GetValues();
    auto& v1 = hf1->GetValues();
    assert(v0.size() == sz && v1.size() == sz);
    std::vector<float> v(sz, 0.0f);

    switch (m_method)
    {
    case Method::Average:
        for (size_t i = 0; i < sz; ++i) {
            v[i] = (v0[i] + v1[i]) * 0.5f;
        }
        break;
    case Method::Add:
        for (size_t i = 0; i < sz; ++i) {
            v[i] = v0[i] + v1[i];
        }
        break;
    case Method::Subtract:
        for (size_t i = 0; i < sz; ++i) {
            v[i] = v0[i] - v1[i];
        }
        break;
    case Method::Multiply:
        for (size_t i = 0; i < sz; ++i) {
            v[i] = v0[i] * v1[i];
        }
        break;
    case Method::Divide:
        for (size_t i = 0; i < sz; ++i) {
            if (v1[i] == 0) {
                v[i] = v0[i];
            } else {
                v[i] = v0[i] / v1[i];
            }
        }
        break;
    case Method::Max:
        for (size_t i = 0; i < sz; ++i) {
            v[i] = std::max(v0[i], v1[i]);
        }
        break;
    case Method::Min:
        for (size_t i = 0; i < sz; ++i) {
            v[i] = std::min(v0[i], v1[i]);
        }
        break;
    default:
        assert(0);
    }

    m_hf = std::make_shared<HeightField>(w, h);
    m_hf->SetValues(v);

    if (m_method == Method::Add) {
        m_hf->Normalize();
    }
}

}
}