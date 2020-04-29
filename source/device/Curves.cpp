#include "terraingraph/device/Curves.h"
#include "terraingraph/DeviceHelper.h"
#include "terraingraph/HeightFieldEval.h"
#include "terraingraph/Context.h"

#include <heightfield/HeightField.h>

namespace terraingraph
{
namespace device
{

void Curves::Execute(const std::shared_ptr<dag::Context>& ctx)
{
    auto prev_hf = DeviceHelper::GetInputHeight(*this, 0);
    if (!prev_hf) {
        return;
    }

    m_hf = std::make_shared<hf::HeightField>(*prev_hf);
    if (m_ctrl_pts.empty()) {
        return;
    }

    auto& dev = *std::static_pointer_cast<Context>(ctx)->ur_dev;

    int32_t min, max;
    CalcHeightRegion(dev, *m_hf, min, max);
    if (min == max) {
        return;
    }

    auto vals = m_hf->GetValues(dev);
    for (auto& v : vals) {
        float v01 = CalcHeight(static_cast<float>(v - min) / (max - min));
        v = static_cast<int32_t>(v01 * (max - min) + min);
    }
    m_hf->SetValues(vals);
}

float Curves::CalcHeight(float h) const
{
    if (m_ctrl_pts.empty()) {
        return h;
    }

    assert(m_type == Type::Linear);
    assert(h >= 0 && h <= 1);

    auto calc = [](const sm::vec2& begin, const sm::vec2& end, float h) -> float
    {
        assert(h >= begin.x && h <= end.x);
        if (begin.x == end.x) {
            return (begin.y + end.y) * 0.5f;
        } else {
            return (h - begin.x) / (end.x - begin.x) * (end.y - begin.y) + begin.y;
        }
    };

    if (h >= 0 && h < m_ctrl_pts.front().x)
    {
        return calc(sm::vec2(0, 0), m_ctrl_pts.front(), h);
    }
    else if (h >= m_ctrl_pts.back().x)
    {
        assert(h <= 1);
        return calc(m_ctrl_pts.back(), sm::vec2(1, 1), h);
    }
    else
    {
        for (size_t i = 0, n = m_ctrl_pts.size(); i < n - 1; ++i) {
            if (h >= m_ctrl_pts[i].x && h < m_ctrl_pts[i + 1].x) {
                return calc(m_ctrl_pts[i], m_ctrl_pts[i + 1], h);
            }
        }
    }

    assert(0);
    return h;
}

void Curves::CalcHeightRegion(const ur::Device& dev, const hf::HeightField& hf,
                              int32_t& min, int32_t& max)
{
    min = std::numeric_limits<int32_t>::max();
    max = -std::numeric_limits<int32_t>::max();
    auto& vals = hf.GetValues(dev);
    for (auto& v : vals)
    {
        if (v < min) {
            min = v;
        }
        if (v > max) {
            max = v;
        }
    }
}

}
}