#include "terr/HeightFieldEval.h"
#include "terr/HeightField.h"

#include <SM_Calc.h>

namespace terr
{

sm::vec2 HeightFieldEval::Gradient(const HeightField& hf, size_t x, size_t y)
{
    if (x >= hf.Width() || y >= hf.Height()) {
        assert(0);
        return sm::vec2();
    }

    sm::vec2 g;

    if (x == 0) {
        g.x = hf.Get(x + 1, y) - hf.Get(x, y);
    } else if (x == hf.Width() - 1) {
        g.x = hf.Get(x, y) - hf.Get(x - 1, y);
    } else {
        g.x = (hf.Get(x + 1, y) - hf.Get(x - 1, y)) / 2;
    }

    if (y == 0) {
        g.y = hf.Get(x, y + 1) - hf.Get(x, y);
    } else if (y == hf.Height() - 1) {
        g.y = hf.Get(x, y) - hf.Get(x, y - 1);
    } else {
        g.y = (hf.Get(x, y + 1) - hf.Get(x, y - 1)) / 2;
    }

	return g;
}

sm::vec3 HeightFieldEval::Normal(const HeightField& hf, size_t x,
                                 size_t y, const sm::vec3& scale)
{
    const auto w = hf.Width();
    const auto h = hf.Height();

    sm::vec3 tot_norm;
    size_t num = 0;

    auto calc = [&](size_t x, size_t y) -> sm::vec3 {
        return sm::vec3(
            static_cast<float>(x),
            hf.Get(x, y),
            static_cast<float>(y)
        ) * scale;
    };

    if (x > 0 && y < h - 1)
    {
        tot_norm += sm::calc_face_normal({
            calc(x, y), calc(x - 1, y + 1), calc(x - 1, y)
        });
        tot_norm += sm::calc_face_normal({
            calc(x, y), calc(x, y + 1), calc(x - 1, y + 1)
        });
        num += 2;
    }
    if (x < w - 1 && y < h - 1)
    {
        tot_norm += sm::calc_face_normal({
            calc(x, y), calc(x + 1, y), calc(x + 1, y + 1)
        });
        tot_norm += sm::calc_face_normal({
            calc(x, y), calc(x + 1, y + 1), calc(x, y + 1)
        });
        num += 2;
    }
    if (x > 0 && y > 0)
    {
        tot_norm += sm::calc_face_normal({
            calc(x, y), calc(x - 1, y), calc(x - 1, y - 1)
        });
        tot_norm += sm::calc_face_normal({
            calc(x, y), calc(x - 1, y - 1), calc(x, y - 1)
        });
        num += 2;
    }
    if (x < w - 1 && y > 0)
    {
        tot_norm += sm::calc_face_normal({
            calc(x, y), calc(x, y - 1), calc(x + 1, y - 1)
        });
        tot_norm += sm::calc_face_normal({
            calc(x, y), calc(x + 1, y - 1), calc(x + 1, y)
        });
        num += 2;
    }
    return tot_norm / static_cast<float>(num);
}

void HeightFieldEval::Region(const HeightField& hf, float& min, float& max)
{
    min = std::numeric_limits<float>::max();
    max = -std::numeric_limits<float>::max();
    auto& vals = hf.GetValues();
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