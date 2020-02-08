#include "wm/Mask.h"

#include <set>

namespace
{

// order
// 3 4 5
// 2   6
// 1 0 7
static const int NEARBY_COUNT = 8;
static const sm::ivec2 NEARBY_OFFSET[NEARBY_COUNT] = {
	sm::ivec2( 0, -1),
	sm::ivec2(-1, -1),
	sm::ivec2(-1,  0),
	sm::ivec2(-1,  1),
	sm::ivec2( 0,  1),
	sm::ivec2( 1,  1),
	sm::ivec2( 1,  0),
	sm::ivec2( 1, -1),
};

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

namespace wm
{

Mask::Mask(size_t width, size_t height)
    : m_width(width)
    , m_height(height)
{
    m_values.resize(m_width * m_height, false);
}

void Mask::SetValues(const std::vector<bool>& values)
{
    assert(m_values.size() == values.size());
    m_values = values;
}

std::vector<std::vector<sm::ivec2>>
Mask::CalcBorders() const
{
    std::vector<std::vector<sm::ivec2>> borders;

    std::vector<bool> visited(m_width * m_height, false);
    auto get_visited = [&](size_t x, size_t y) {
        return visited[y * m_width + x];
    };
    auto set_visited = [&](size_t x, size_t y) {
        visited[y * m_width + x] = true;
    };

    for (size_t y = 0; y < m_height; ++y) {
        for (size_t x = 0; x < m_width; ++x) {
            if (get_visited(x, y) ||
                !IsPixelBorder(x, y)) {
                continue;
            }

            sm::ivec2 first_pos(x, y);
            sm::ivec2 curr_pos(first_pos);
            std::vector<sm::ivec2> border;
            border.push_back(curr_pos);
            int curr_dir = -1;
            do {
		        int next_dir;
		        if (curr_dir == -1) {
			        // the first, start from [0]
			        next_dir = 0;
		        } else {
			        // curr dir is next of inverse last dir
			        next_dir = (curr_dir + (QUERY_COUNT >> 1)) + 1;
		        }
                for (int i = 0; i < QUERY_COUNT; ++i)
                {
                    int dir = (i + next_dir) % QUERY_COUNT;
                    auto nearby = curr_pos + QUERY_OFFSET[dir];
                    if (nearby.x < 0 || nearby.x >= static_cast<int>(m_width) ||
                        nearby.y < 0 || nearby.y >= static_cast<int>(m_height) ||
                        get_visited(nearby.x, nearby.y)) {
                        continue;
                    }

                    if (IsPixelBorder(nearby.x, nearby.y))
                    {
                        curr_dir = dir;
                        curr_pos = nearby;
                        border.push_back(curr_pos);
                        set_visited(curr_pos.x, curr_pos.y);
                        break;
                    }
                }
            } while (curr_pos != first_pos);
            assert(!border.empty());
            borders.push_back(border);
        }
    }

    return borders;
}

bool Mask::IsPixelBorder(size_t x, size_t y) const
{
	if (x < 0 || x >= m_width ||
		y < 0 || y >= m_height) {
		return false;
	}

	if (!IsPixelMasked(x, y)) {
		return false;
	}

	for (int i = 0; i < NEARBY_COUNT; ++i)
    {
        auto near_x = x + NEARBY_OFFSET[i].x;
        auto near_y = y + NEARBY_OFFSET[i].y;
        if (near_x < 0 || near_x >= m_width ||
            near_y < 0 || near_y >= m_height) {
            return true;
        }

		if (!IsPixelMasked(near_x, near_y)) {
			return true;
		}
	}
	return false;
}

bool Mask::IsPixelMasked(size_t x, size_t y) const
{
    return m_values[y * m_width + x];
}

}