#include "terraingraph/device/AutoGen.h"
#include "terraingraph/DeviceHelper.h"
#include "terraingraph/Mask.h"
#include "terraingraph/Utility.h"

#include <SM_Calc.h>
#include <heightfield/HeightField.h>

namespace
{

// order
// 3 4 5
// 2   6
// 1 0 7
static const int QUERY_COUNT = 8;
static const sm::ivec2 QUERY_OFFSET[QUERY_COUNT] = {
	sm::ivec2( 0, -1),
	sm::ivec2(-1, -1),
	sm::ivec2(-1,  0),
	sm::ivec2(-1,  1),
	sm::ivec2( 0,  1),
	sm::ivec2( 1,  1),
	sm::ivec2( 1,  0),
	sm::ivec2( 1, -1),
};

}

namespace terraingraph
{
namespace device
{

void AutoGen::Execute(const std::shared_ptr<dag::Context>& ctx)
{
    auto mask = DeviceHelper::GetInputMask(*this, 0);
    if (!mask) {
        return;
    }

    m_hf = MaskToHeightField(*mask);
}

std::shared_ptr<hf::HeightField>
AutoGen::MaskToHeightField(const Mask& mask) const
{
    std::vector<float> dists(m_width * m_height, 0);

    auto hf = std::make_shared<hf::HeightField>(m_width, m_height);

    std::vector<float> dist_map;
    InitDistMap(mask, dist_map);
    SplitDistMap(mask, dist_map);
    BuildDistMap(mask, dist_map);

    auto mw = mask.Width();
    auto mh = mask.Height();
    std::vector<float> heights(m_width * m_height, 0.0f);
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            float mx = static_cast<float>(x) / m_width * mw;
            float my = static_cast<float>(y) / m_height * mh;
            size_t ix = static_cast<size_t>(std::floor(mx));
            size_t iy = static_cast<size_t>(std::floor(my));
            float fx = mx - ix;
            float fy = my - iy;
            float h;

            h = dist_map[mw * iy + ix];
            if (h == -1) {
                h = 0;
            }

            if (ix == mw - 1 && iy == mh - 1)
            {
                h = dist_map[mw * iy + ix];
            }
            else if (ix == mw - 1)
            {
                h = dist_map[mw * iy + ix] * (1 - fy) +
                    dist_map[mw * (iy + 1) + ix] * fy;
            }
            else if (iy == mh - 1)
            {
                h = dist_map[mw * iy + ix] * (1 - fx) +
                    dist_map[mw * iy + ix + 1] * fx;
            }
            else
            {
                h = dist_map[mw * iy + ix] * (1 - fx) * (1 - fy) +
                    dist_map[mw * iy + ix + 1] * fx * (1 - fy) +
                    dist_map[mw * (iy + 1) + ix] * (1 - fx) * fy +
                    dist_map[mw * (iy + 1) + ix + 1] * fx * fy;
            }
            heights[m_width * y + x] = h;
        }
    }

    hf->SetValues(heights);
    hf->Normalize();

    return hf;
}

void AutoGen::InitDistMap(const Mask& mask, std::vector<float>& dist_map) const
{
    auto w = mask.Width();
    auto h = mask.Height();
    auto& vals = mask.GetValues();

    dist_map.resize(w * h, 0);
    for (size_t i = 0, n = w * h; i < n; ++i)
    {
        if (vals[i]) {
            dist_map[i] = std::numeric_limits<float>::max();
        } else {
            dist_map[i] = 0;
            continue;
        }
        if (mask.IsPixelBorder(i % w, i / w)) {
            dist_map[i] = 1.0f;
        }
    }
}

void AutoGen::SplitDistMap(const Mask& mask, std::vector<float>& dist_map) const
{
    auto w = mask.Width();
    auto h = mask.Height();
    assert(dist_map.size() == w * h);

    // build connection groups
    int group_id = 0;
    std::vector<int> group_centers;
    std::vector<int> group_ids(dist_map.size(), -1);
    std::vector<std::vector<size_t>> groups;
    for (size_t i = 0, n = dist_map.size(); i < n; ++i)
    {
        if (dist_map[i] == 0 || group_ids[i] != -1) {
            continue;
        }

        std::vector<size_t> group;

        std::set<size_t> buf;
        buf.insert(i);
        group.push_back(i);
        while (!buf.empty())
        {
            std::set<size_t> next_buf;
            for (auto& i : buf)
            {
                group_ids[i] = group_id;

                sm::ivec2 curr_pos(i % w, i / w);
                for (int j = 0; j < QUERY_COUNT; ++j)
                {
                    auto nearby = curr_pos + QUERY_OFFSET[j];
                    if (nearby.x < 0 || nearby.x >= static_cast<int>(w) ||
                        nearby.y < 0 || nearby.y >= static_cast<int>(h)) {
                        continue;
                    }

                    auto next = w * nearby.y + nearby.x;
                    if (dist_map[next] == 0) {
                        continue;
                    }
                    assert(group_ids[next] == -1 || group_ids[next] == group_id);
                    if (group_ids[next] == -1)
                    {
                        group_ids[next] = group_id;
                        next_buf.insert(next);
                        group.push_back(next);
                    }
                }
            }
            buf = next_buf;
        }

        groups.push_back(group);
        group_centers.push_back(-1);

        ++group_id;
    }

    // split
    size_t max_group_sz = static_cast<size_t>(w * h * m_group_cap);
    for (auto& group : groups)
    {
        if (group.empty() || static_cast<int>(group.size()) < max_group_sz) {
            continue;
        }

        int num = static_cast<int>(std::ceil(static_cast<float>(group.size()) / max_group_sz));

        std::vector<int> sub_group_centers;

        // init centers
        std::set<size_t> centers;
        while (static_cast<int>(centers.size()) < num)
        {
            size_t idx = static_cast<size_t>(Utility::RangedRandom(0.0f, static_cast<float>(group.size() - 1)));
            centers.insert(group[idx]);
        }

        size_t count = 0;
        while (count++ < 1024)
        {
            std::vector<std::vector<size_t>> sub_groups(centers.size());
            for (auto& i : group)
            {
                float min_dist = std::numeric_limits<float>::max();
                int min_idx = -1;

                size_t c_idx = 0;
                for (auto& c : centers)
                {
                    float dist = sm::dis_square_pos_to_pos(
                        { static_cast<float>(i % w), static_cast<float>(i / w) },
                        { static_cast<float>(c % w), static_cast<float>(c / w) }
                    );
                    if (dist < min_dist)
                    {
                        min_dist = dist;
                        min_idx = c_idx;
                    }
                    ++c_idx;
                }
                assert(min_idx >= 0 && min_idx < sub_groups.size());
                sub_groups[min_idx].push_back(i);
            }

            sub_group_centers.clear();

            std::set<size_t> new_centers;
            for (auto& sub_group : sub_groups)
            {
                size_t cx = 0, cy = 0;
                for (auto& i : sub_group) {
                    cx += i % w;
                    cy += i / w;
                }
                cx /= sub_group.size();
                cy /= sub_group.size();
                size_t idx = cy * w + cx;
                new_centers.insert(idx);
                sub_group_centers.push_back(idx);
            }

            if (new_centers == centers)
            {
                for (auto& sub_group : sub_groups)
                {
                    for (auto& i : sub_group) {
                        group_ids[i] = group_id;
                    }
                    ++group_id;
                }
                break;
            }
            else
            {
                centers = new_centers;
            }
        }

        std::copy(sub_group_centers.begin(), sub_group_centers.end(), std::back_inserter(group_centers));
    }

    // set border flag
    assert(group_centers.size() == group_id);
    for (size_t i = 0, n = dist_map.size(); i < n; ++i)
    {
        if (dist_map[i] == 1.0f || dist_map[i] == 0) {
            continue;
        }
        sm::ivec2 curr_pos(i % w, i / w);
        for (int j = 0; j < QUERY_COUNT; ++j)
        {
            auto nearby = curr_pos + QUERY_OFFSET[j];
            if (nearby.x < 0 || nearby.x >= static_cast<int>(w) ||
                nearby.y < 0 || nearby.y >= static_cast<int>(h)) {
                continue;
            }

            auto next = w * nearby.y + nearby.x;
            if (dist_map[next] == 0) {
                dist_map[i] = 1.0f;
                continue;
            }
            if (group_ids[i] != group_ids[next])
            {
                //dist_map[i] = 1.0f;
                //dist_map[next] = 1.0f;

                dist_map[i]    = 2;
                dist_map[next] = 2;

                auto g0 = group_ids[i];
                auto g1 = group_ids[next];
                assert(g0 >= 0 && g0 < group_centers.size());
                assert(g1 >= 0 && g1 < group_centers.size());
                auto c0 = group_centers[g0];
                auto c1 = group_centers[g1];
                assert(c0 >= 0 && c1 >= 0);

                sm::vec2 p0(static_cast<float>(i % w), static_cast<float>(i / w));
                sm::vec2 p1(static_cast<float>(next % w), static_cast<float>(next / w));
                sm::vec2 pc0(static_cast<float>(c0 % w), static_cast<float>(c0 / w));
                sm::vec2 pc1(static_cast<float>(c1 % w), static_cast<float>(c1 / w));

                auto ang = sm::get_angle_in_direction(p0, pc0, pc1);
                if (ang > SM_PI * 0.7f && ang < SM_PI * 1.3f) {
                    dist_map[i] = 2.99;
                    dist_map[next] = 2.99;
                }

                continue;
            }
        }
    }

    // set conn small hill

}

void AutoGen::BuildDistMap(const Mask& mask, std::vector<float>& dist_map) const
{
    auto w = mask.Width();
    auto h = mask.Height();
    auto& vals = mask.GetValues();
    assert(dist_map.size() == w * h);

    std::set<size_t> buf;

    // insert border
    for (size_t i = 0, n = dist_map.size(); i < n; ++i) {
        if (dist_map[i] > 0 &&
            dist_map[i] < std::numeric_limits<float>::max()) {
            buf.insert(i);
        }
    }

    // raise inner
    while (!buf.empty())
    {
        std::set<size_t> next_buf;
        for (auto& i : buf)
        {
            sm::ivec2 curr_pos(i % w, i / w);
            float next_dist = dist_map[i] + 1.0f;
            for (int j = 0; j < QUERY_COUNT; ++j)
            {
                auto nearby = curr_pos + QUERY_OFFSET[j];
                if (nearby.x < 0 || nearby.x >= static_cast<int>(w) ||
                    nearby.y < 0 || nearby.y >= static_cast<int>(h)) {
                    continue;
                }

                auto next = w * nearby.y + nearby.x;
                if (!vals[next]) {
                    continue;
                }

                if (next_dist < dist_map[next]) {
                    dist_map[next] = next_dist;
                    next_buf.insert(next);
                }
            }
        }
        buf = next_buf;
    }
}

}
}