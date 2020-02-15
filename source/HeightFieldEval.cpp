#include "wm/HeightFieldEval.h"
#include "wm/HeightField.h"

#include <SM_Calc.h>

#include <array>

namespace wm
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

ScalarField2D HeightFieldEval::DrainageArea(const HeightField& hf)
{
    auto w = hf.Width();
    auto h = hf.Height();

	// Sort all point by decreasing height
	std::deque<ScalarValue> points;
    for (size_t y = 0; y < h; ++y) {
        for (size_t x = 0; x < w; ++x) {
            points.push_back(ScalarValue(x, y, hf.Get(x, y)));
        }
    }
	std::sort(points.begin(), points.end(), [](ScalarValue p1, ScalarValue p2) { return p1.value > p2.value; });

	std::array<float, 8> slopes;
	std::array<sm::ivec2, 8> coords;
	ScalarField2D DA = ScalarField2D(w, h, 1.0f);
	while (!points.empty())
	{
		ScalarValue p = points.front();
		points.pop_front();

		slopes.fill(0.0f);
		size_t i = p.x, j = p.y;
		float h = hf.Get(i, j);
		int neighbour_count = 0;
		for (int k = -1; k <= 1; k++)
		{
			for (int l = -1; l <= 1; l++)
			{
				if ((k == 0 && l == 0) || hf.Inside(i + k, j + l) == false)
					continue;
				// If current point has lower neighbour : compute slope to later distribute accordingly.
				float nH = hf.Get(i + k, j + l);
				if (h > nH)
				{
					float dH = h - nH;
					if (k + l == -1 || k + l == 1)
						slopes[neighbour_count] = dH;
					else
						slopes[neighbour_count] = dH / sqrt(2.0f);

					coords[neighbour_count] = sm::ivec2(i + k, j + l);
					neighbour_count++;
				}
			}
		}

		// Distribute to those lower neighbours
        float sum = 0;
        for (auto& s : slopes) {
            sum += s;
        }
        for (int k = 0; k < neighbour_count; k++) {
            size_t x = static_cast<size_t>(coords[k].x);
            size_t y = static_cast<size_t>(coords[k].y);
            DA.Set(x, y, DA.Get(x, y) + DA.Get(i, j) * (slopes[k] / sum));
        }
	}
	return DA;
}

ScalarField2D HeightFieldEval::Wetness(const HeightField& hf)
{
    ScalarField2D DA = DrainageArea(hf);
    ScalarField2D S = Slope(hf);
    for (size_t y = 0, h = hf.Height(); y < h; ++y) {
        for (size_t x = 0, w = hf.Width(); x < w; ++x) {
            DA.Set(x, y, abs(log(DA.Get(x, y) / (1.0f + S.Get(x, y)))));
        }
    }
    return DA;
}

// Compute the StreamPower field, as described by http://geosci.uchicago.edu/~kite/doc/Whipple_and_Tucker_1999.pdf.
ScalarField2D HeightFieldEval::StreamPower(const HeightField& hf)
{
    ScalarField2D DA = DrainageArea(hf);
    ScalarField2D S = Slope(hf);
    for (size_t y = 0, h = hf.Height(); y < h; ++y) {
        for (size_t x = 0, w = hf.Width(); x < w; ++x) {
            DA.Set(x, y, sqrt(DA.Get(x, y)) * S.Get(x, y));
        }
    }
    return DA;
}

ScalarField2D HeightFieldEval::Slope(const HeightField& hf)
{
    auto w = hf.Width();
    auto h = hf.Height();
    ScalarField2D S = ScalarField2D(w, h);
    for (size_t y = 0; y < h; ++y) {
        for (size_t x = 0; x < w; ++x) {
            S.Set(x, y, Gradient(hf, x, y).Length());
        }
    }
    return S;
}

}