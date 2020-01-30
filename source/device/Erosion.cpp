#include "terr/device/Erosion.h"
#include "terr/Context.h"
#include "terr/HeightField.h"
#include "terr/DeviceHelper.h"
#include "terr/HeightFieldEval.h"

#include <SM_Vector.h>

#include <vector>

namespace terr
{
namespace device
{

void Erosion::Execute(const Context& ctx)
{
    bool new_hf = m_hf == nullptr;

    auto prev_hf = DeviceHelper::GetInputHeight(*this, 0);
    if (!prev_hf) {
        return;
    }

    m_hf = std::make_shared<HeightField>(*prev_hf);
    if (new_hf || m_map_sz != ctx.size) {
        InitializeBrushIndices(ctx.size);
    }

    const size_t max_droplet_lifetime = 30;
    const float init_water_volume = 1;
    const float init_speed = 1;

    for (size_t i = 0; i < static_cast<size_t>(m_iter); ++i)
    {
        // Create water droplet at random point on map
        sm::vec2 pos;
        pos.x = static_cast<float>(rand()) / RAND_MAX * (ctx.size - 1);
        pos.y = static_cast<float>(rand()) / RAND_MAX * (ctx.size - 1);
        sm::vec2 dir(0, 0);
        float speed = init_speed;
        float water = init_water_volume;
        float sediment = 0;

        for (int lifetime = 0; lifetime < max_droplet_lifetime; lifetime++)
        {
            const size_t node_x = static_cast<size_t>(pos.x);
            const size_t node_y = static_cast<size_t>(pos.y);
            const size_t droplet_idx = node_y * ctx.size + node_x;
            const float cell_off_x = pos.x - node_x;
            const float cell_off_y = pos.y - node_y;

            const auto gradient = HeightFieldEval::Gradient(*m_hf, node_x, node_y);

            // calc next pos
            dir.x = (dir.x - gradient.x) * m_ki + gradient.x;
            dir.y = (dir.y - gradient.y) * m_ki + gradient.y;
            dir.Normalize();
            pos += dir;

            // Stop simulating droplet if it's not moving or has flowed over edge of map
            if ((dir.x == 0 && dir.y == 0) ||
                pos.x < 0 || pos.x >= ctx.size - 1 ||
                pos.y < 0 || pos.y >= ctx.size - 1) {
                break;
            }

            // Find the droplet's new height and calculate the dh
            float new_h = m_hf->Get(pos.x, pos.y);
            float dh = new_h - m_hf->Get(node_x, node_y);

            // Calculate the droplet's sediment capacity (higher when moving fast down a slope and contains lots of water)
            float sediment_cap = std::max(-dh * speed * water * m_kq, m_min_slope);

            // If carrying more sediment than capacity, or if flowing uphill:
            if (sediment > sediment_cap || dh > 0)
            {
                // If moving uphill (dh > 0) try fill up to the current height, otherwise deposit a fraction of the excess sediment
                float amount_to_deposit = (dh > 0) ? std::min(dh, sediment) : (sediment - sediment_cap) * m_kd;
                sediment -= amount_to_deposit;

                // Add the sediment to the four nodes of the current cell using bilinear interpolation
                // Deposition is not distributed over a radius (like erosion) so that it can fill small pits
                m_hf->Add(node_x,     node_y,     amount_to_deposit * (1 - cell_off_x) * (1 - cell_off_y));
                m_hf->Add(node_x + 1, node_y,     amount_to_deposit * cell_off_x * (1 - cell_off_y));
                m_hf->Add(node_x,     node_y + 1, amount_to_deposit * (1 - cell_off_x) * cell_off_y);
                m_hf->Add(node_x + 1, node_y + 1, amount_to_deposit * cell_off_x * cell_off_y);
            }
            else
            {
                // Erode a fraction of the droplet's current carry capacity.
                // Clamp the erosion to the change in height so that it doesn't dig a hole in the terrain behind the droplet
                float amount_to_erode = std::min((sediment_cap - sediment) * m_kr, -dh);

                // Use erosion brush to erode from all nodes inside the droplet's erosion radius
                for (size_t brush_pt_idx = 0; brush_pt_idx < m_brush_indices[droplet_idx].size(); ++brush_pt_idx)
                {
                    size_t node_idx = m_brush_indices[droplet_idx][brush_pt_idx];
                    float weighed_erode_amount = amount_to_erode * m_brush_weights[droplet_idx][brush_pt_idx];
                    float d_sediment = (m_hf->Get(node_idx) < weighed_erode_amount) ? m_hf->Get(node_idx) : weighed_erode_amount;
                    m_hf->Add(node_idx, -d_sediment);
                    sediment += d_sediment;
                }
            }

            // Update droplet's speed and water content
            speed = std::sqrt(speed * speed + dh * m_g);
            water *= (1 - m_kw);
        }
    }
}

void Erosion::InitializeBrushIndices(size_t size)
{
    m_map_sz = size;

    const size_t tot_sz = m_map_sz * m_map_sz;
    m_brush_indices.resize(tot_sz);
    m_brush_weights.resize(tot_sz);

    std::vector<int> x_offsets(tot_sz * 4);
    std::vector<int> y_offsets(tot_sz * 4);
    std::vector<float> weights(tot_sz * 4);

    float weight_sum = 0;
    int add_index = 0;

    for (size_t i = 0; i < tot_sz; ++i)
    {
        int cx = i % m_map_sz;
        int cy = i / m_map_sz;

        if (cy <= m_radius || cy >= m_map_sz - m_radius || cx <= m_radius + 1 || cx >= m_map_sz - m_radius)
        {
            weight_sum = 0;
            add_index = 0;
            const int ir = static_cast<int>(m_radius);
            for (int y = -ir; y <= ir; y++)
            {
                for (int x = -ir; x <= ir; x++)
                {
                    float sqr_dst = static_cast<float>(x * x + y * y);
                    if (sqr_dst < m_radius * m_radius)
                    {
                        int coord_x = cx + x;
                        int coord_y = cy + y;

                        if (coord_x >= 0 && static_cast<size_t>(coord_x) < m_map_sz &&
                            coord_y >= 0 && static_cast<size_t>(coord_y) < m_map_sz)
                        {
                            float weight = 1 - std::sqrt(sqr_dst) / m_radius;
                            weight_sum += weight;
                            weights[add_index] = weight;
                            x_offsets[add_index] = x;
                            y_offsets[add_index] = y;
                            add_index++;
                        }
                    }
                }
            }
        }

        int num_entries = add_index;
        m_brush_indices[i].resize(num_entries);
        m_brush_weights[i].resize(num_entries);
        for (int j = 0; j < num_entries; ++j) {
            m_brush_indices[i][j] = (y_offsets[j] + cy) * m_map_sz + x_offsets[j] + cx;
            m_brush_weights[i][j] = weights[j] / weight_sum;
        }
    }
}

}
}