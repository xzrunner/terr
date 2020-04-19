// from SebLague/Hydraulic-Erosion

#include "terraingraph/device/Erosion.h"
#include "terraingraph/DeviceHelper.h"
#include "terraingraph/HeightFieldEval.h"
#include "terraingraph/EvalGPU.h"
#include "terraingraph/Utility.h"
#include "terraingraph/Context.h"

#include <SM_Vector.h>
#include <heightfield/HeightField.h>
#include <unirender2/ShaderProgram.h>
#include <unirender2/Device.h>
#include <unirender2/ComputeBuffer.h>
#include <painting0/ShaderUniforms.h>

#include <vector>

namespace
{

std::shared_ptr<terraingraph::EvalGPU> EVAL = nullptr;

const char* cs = R"(

#version 430

layout(std430, binding=0) coherent buffer HeightfieldDataFloat { float heights[]; };
layout(std430, binding=1) buffer RandomIndicesInt { int random_indices[]; };
layout(std430, binding=2) buffer BrushIndicesInt { int brush_indices[]; };
layout(std430, binding=3) buffer BrushWeightsFloat { float brush_weights[]; };

uniform int map_size;
uniform int brush_length;
uniform int border_size;

uniform int max_lifetime;
uniform float inertia;
uniform float sediment_capacity_factor;
uniform float min_sediment_cap;
uniform float deposit_spd;
uniform float erode_spd;

uniform float evaporate_spd;
uniform float gravity;
uniform float start_speed;
uniform float start_water;

vec3 CalculateHeightAndGradient(float pos_x, float pos_y)
{
    int coord_x = int(pos_x);
    int coord_y = int(pos_y);

    // Calculate droplet's offset inside the cell (0,0) = at NW node, (1,1) = at SE node
    float x = pos_x - coord_x;
    float y = pos_y - coord_y;

    // Calculate heights of the four nodes of the droplet's cell
    int node_idx_nw = coord_y * map_size + coord_x;
    float height_nw = heights[node_idx_nw];
    float height_ne = heights[node_idx_nw + 1];
    float height_sw = heights[node_idx_nw + map_size];
    float height_se = heights[node_idx_nw + map_size + 1];

    // Calculate droplet's direction of flow with bilinear interpolation of height difference along the edges
    float gradient_x = (height_ne - height_nw) * (1 - y) + (height_se - height_sw) * y;
    float gradient_y = (height_sw - height_nw) * (1 - x) + (height_se - height_ne) * x;

    // Calculate height with bilinear interpolation of the heights of the nodes of the cell
    float height = height_nw * (1 - x) * (1 - y) + height_ne * x * (1 - y) + height_sw * (1 - x) * y + height_se * x * y;

    return vec3(gradient_x, gradient_y, height);
}

layout(local_size_x = 1024) in;
void main()
{
    int index = random_indices[gl_GlobalInvocationID.x];
    float pos_x = float(index % map_size);
    float pos_y = float(index / map_size);
    float dir_x = 0;
    float dir_y = 0;
    float speed = start_speed;
    float water = start_water;
    float sediment = 0;

    for (int lifetime = 0; lifetime < max_lifetime; lifetime ++) {
        int node_x = int(pos_x);
        int node_y = int(pos_y);
        int droplet_index = node_y * map_size + node_x;
        // Calculate droplet's offset inside the cell (0,0) = at NW node, (1,1) = at SE node
        float cell_offset_x = pos_x - node_x;
        float cell_offset_y = pos_y - node_y;

        // Calculate droplet's height and direction of flow with bilinear interpolation of surrounding heights
        vec3 height_and_gradient = CalculateHeightAndGradient (pos_x, pos_y);

        // Update the droplet's direction and position (move position 1 unit regardless of speed)
        dir_x = (dir_x * inertia - height_and_gradient.x * (1 - inertia));
        dir_y = (dir_y * inertia - height_and_gradient.y * (1 - inertia));
        // Normalize direction
        float len = max(0.01, sqrt(dir_x * dir_x + dir_y * dir_y));
        dir_x /= len;
        dir_y /= len;
        pos_x += dir_x;
        pos_y += dir_y;

        // Stop simulating droplet if it's not moving or has flowed over edge of heights
        if ((dir_x == 0 && dir_y == 0) || pos_x < border_size || pos_x > map_size - border_size || pos_y < border_size || pos_y > map_size - border_size) {
            break;
        }

        // Find the droplet's new height and calculate the dh
        float new_h = CalculateHeightAndGradient (pos_x, pos_y).z;
        float dh = new_h - height_and_gradient.z;

        // Calculate the droplet's sediment capacity (higher when moving fast down a slope and contains lots of water)
        float sediment_cap = max(-dh * speed * water * sediment_capacity_factor, min_sediment_cap);

        // If carrying more sediment than capacity, or if flowing uphill:
        if (sediment > sediment_cap || dh > 0) {
            // If moving uphill (dh > 0) try fill up to the current height, otherwise deposit a fraction of the excess sediment
            float amount_to_deposit = (dh > 0) ? min(dh, sediment) : (sediment - sediment_cap) * deposit_spd;
            sediment -= amount_to_deposit;

            // Add the sediment to the four nodes of the current cell using bilinear interpolation
            // Deposition is not distributed over a radius (like erosion) so that it can fill small pits
            heights[droplet_index] += amount_to_deposit * (1 - cell_offset_x) * (1 - cell_offset_y);
            heights[droplet_index + 1] += amount_to_deposit * cell_offset_x * (1 - cell_offset_y);
            heights[droplet_index + map_size] += amount_to_deposit * (1 - cell_offset_x) * cell_offset_y;
            heights[droplet_index + map_size + 1] += amount_to_deposit * cell_offset_x * cell_offset_y;
        }
        else {
            // Erode a fraction of the droplet's current carry capacity.
            // Clamp the erosion to the change in height so that it doesn't dig a hole in the terrain behind the droplet
            float amount_to_erode = min ((sediment_cap - sediment) * erode_spd, -dh);

            for (int i = 0; i < brush_length; i ++) {
                int erode_idx = droplet_index + brush_indices[i];

                float weighted_erode_amount = amount_to_erode * brush_weights[i];
                float delta_sediment = (heights[erode_idx] < weighted_erode_amount) ? heights[erode_idx] : weighted_erode_amount;
                heights[erode_idx] -= delta_sediment;
                sediment += delta_sediment;
            }
        }

        // Update droplet's speed and water content
        speed = sqrt(max(0, speed * speed + dh * gravity));
        water *= (1 - evaporate_spd);
    }

}

)";

}

namespace terraingraph
{
namespace device
{

void Erosion::Execute(const std::shared_ptr<dag::Context>& ctx)
{
    bool new_hf = m_hf == nullptr;

    auto prev_hf = DeviceHelper::GetInputHeight(*this, 0);
    if (!prev_hf) {
        return;
    }

    m_hf = std::make_shared<hf::HeightField>(*prev_hf);

#ifdef EROSION_GPU
    RunGPU(ctx);
#else
    RunCPU();
#endif // EROSION_GPU
}

#ifdef EROSION_GPU

void Erosion::RunGPU(const std::shared_ptr<dag::Context>& ctx)
{
    int map_size = m_hf->Width();
    int erosion_brush_radius = 3;
    int map_size_with_border = m_hf->Width() + erosion_brush_radius * 2;

    int num_threads = m_iter / 1024;

    auto& dev = *std::static_pointer_cast<Context>(ctx)->ur_dev;

    // Bind shader
    if (!EVAL) {
        EVAL = std::make_shared<EvalGPU>(dev, cs);
    }
    EVAL->GetShader();

    // Create brush
    std::vector<int> brush_index_offsets;
    std::vector<float> brush_weights;
    float weight_sum = 0;
    for (int brush_y = -erosion_brush_radius; brush_y <= erosion_brush_radius; brush_y++) {
        for (int brush_x = -erosion_brush_radius; brush_x <= erosion_brush_radius; brush_x++) {
            float sqr_dst = static_cast<float>(brush_x * brush_x + brush_y * brush_y);
            if (sqr_dst < erosion_brush_radius * erosion_brush_radius) {
                brush_index_offsets.push_back(brush_y * map_size + brush_x);
                float brush_weight = 1 - std::sqrt(sqr_dst) / erosion_brush_radius;
                weight_sum += brush_weight;
                brush_weights.push_back(brush_weight);
            }
        }
    }
    for (auto& w : brush_weights) {
        w /= weight_sum;
    }

    // Send brush data to compute shader
    auto brush_index_buf = dev.CreateComputeBuffer(brush_index_offsets, 2);
    auto brush_weight_buf = dev.CreateComputeBuffer(brush_weights, 3);

    // Generate random indices for droplet placement
    std::vector<int> random_indices(m_iter);
    for (int i = 0; i < m_iter; i++)
    {
        //const float min = static_cast<float>(erosionBrushRadius);
        //const float max = static_cast<float>(map_size + erosionBrushRadius);
        const float min = 0.0f;
        const float max = static_cast<float>(map_size);
        int randomX = static_cast<int>(Utility::RangedRandom(min, max));
        int randomY = static_cast<int>(Utility::RangedRandom(min, max));
        random_indices[i] = randomY * map_size + randomX;
    }

    // Send random indices to compute shader
    auto random_idx_buf = dev.CreateComputeBuffer(random_indices, 1);

    // Heightmap buffer
    auto heights = m_hf->GetValues(dev);
    auto heights_buf = dev.CreateComputeBuffer(heights, 0);

    // Uniforms
    pt0::ShaderUniforms vals;
    vals.AddVar("border_size",              pt0::RenderVariant(/*erosionBrushRadius*/0));
    vals.AddVar("map_size",                 pt0::RenderVariant(/*map_size_with_border*/map_size));
    vals.AddVar("brush_length",             pt0::RenderVariant(static_cast<int>(brush_index_offsets.size())));
    vals.AddVar("max_lifetime",             pt0::RenderVariant(30));
    vals.AddVar("inertia",                  pt0::RenderVariant(m_ki));
    vals.AddVar("sediment_capacity_factor", pt0::RenderVariant(m_kq));
    vals.AddVar("min_sediment_cap",         pt0::RenderVariant(m_min_slope));
    vals.AddVar("deposit_spd",              pt0::RenderVariant(m_kd));
    vals.AddVar("erode_spd",                pt0::RenderVariant(m_kr));
    vals.AddVar("evaporate_spd",            pt0::RenderVariant(m_kw));
    vals.AddVar("gravity",                  pt0::RenderVariant(m_g));
    vals.AddVar("start_speed",              pt0::RenderVariant(1.0f));
    vals.AddVar("start_water",              pt0::RenderVariant(1.0f));
    vals.Bind(*EVAL->GetShader());

    // Run compute shader
    dev.DispatchCompute(num_threads);

    // Update CPU data
    heights_buf->GetComputeBufferData(heights);
    m_hf->SetValues(heights);

    // Release buffers
    //rc.BindShader(0);
}

#else

void Erosion::RunCPU()
{
    size_t w = m_hf->Width();
    size_t h = m_hf->Height();
    InitializeBrushIndices(w, h);

    const size_t max_droplet_lifetime = 30;
    const float init_water_volume = 1;
    const float init_speed = 1;

    for (size_t i = 0; i < static_cast<size_t>(m_iter); ++i)
    {
        // Create water droplet at random point on heights
        sm::vec2 pos;
        pos.x = static_cast<float>(rand()) / RAND_MAX * (w - 1);
        pos.y = static_cast<float>(rand()) / RAND_MAX * (h - 1);
        sm::vec2 dir(0, 0);
        float speed = init_speed;
        float water = init_water_volume;
        float sediment = 0;

        for (int lifetime = 0; lifetime < max_droplet_lifetime; lifetime++)
        {
            const size_t node_x = static_cast<size_t>(pos.x);
            const size_t node_y = static_cast<size_t>(pos.y);
            const size_t droplet_idx = node_y * w + node_x;
            const float cell_off_x = pos.x - node_x;
            const float cell_off_y = pos.y - node_y;

            const auto gradient = HeightFieldEval::Gradient(*m_hf, node_x, node_y);

            // calc next pos
            //dir.x = (dir.x - gradient.x) * m_ki + gradient.x;
            //dir.y = (dir.y - gradient.y) * m_ki + gradient.y;
            dir.x = (dir.x * m_ki - gradient.x * (1 - m_ki));
            dir.y = (dir.y * m_ki - gradient.y * (1 - m_ki));

            // Stop simulating droplet if it's not moving
            if (dir.x == 0 && dir.y == 0) {
                break;
            }

            dir.Normalize();
            pos += dir;

            // Stop simulating droplet if has flowed over edge of heights
            if (pos.x < 0 || pos.x >= w - 1 ||
                pos.y < 0 || pos.y >= h - 1) {
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
                if (sediment != 0)
                {
                    m_hf->Add(node_x,     node_y,     amount_to_deposit * (1 - cell_off_x) * (1 - cell_off_y));
                    m_hf->Add(node_x + 1, node_y,     amount_to_deposit * cell_off_x * (1 - cell_off_y));
                    m_hf->Add(node_x,     node_y + 1, amount_to_deposit * (1 - cell_off_x) * cell_off_y);
                    m_hf->Add(node_x + 1, node_y + 1, amount_to_deposit * cell_off_x * cell_off_y);
                }
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
            auto spd_square = speed * speed + dh * m_g;
            speed = spd_square > 0 ? std::sqrt(spd_square) : 0;
            water *= (1 - m_kw);
        }
    }
}

#endif // EROSION_GPU

#ifndef EROSION_GPU

void Erosion::InitializeBrushIndices(size_t width, size_t height)
{
    const size_t tot_sz = width * height;
    m_brush_indices.resize(tot_sz);
    m_brush_weights.resize(tot_sz);

    std::vector<int> x_offsets(tot_sz * 4);
    std::vector<int> y_offsets(tot_sz * 4);
    std::vector<float> weights(tot_sz * 4);

    float weight_sum = 0;
    int add_index = 0;

    for (size_t i = 0; i < tot_sz; ++i)
    {
        int cx = i % width;
        int cy = i / width;

        if (cy <= m_radius || cy >= height - m_radius || cx <= m_radius + 1 || cx >= width - m_radius)
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

                        if (coord_x >= 0 && static_cast<size_t>(coord_x) < width &&
                            coord_y >= 0 && static_cast<size_t>(coord_y) < height)
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
            m_brush_indices[i][j] = (y_offsets[j] + cy) * width + x_offsets[j] + cx;

            auto w = weights[j] / weight_sum;
            m_brush_weights[i][j] = weights[j] / weight_sum;
        }
    }
}

#endif // EROSION_GPU

}
}