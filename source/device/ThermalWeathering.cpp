#include "wm/device/ThermalWeathering.h"
#include "wm/DeviceHelper.h"
#include "wm/HeightField.h"
#include "wm/EvalGPU.h"

#include <unirender/Blackboard.h>
#include <painting0/ShaderUniforms.h>

namespace
{

std::shared_ptr<wm::EvalGPU> EVAL = nullptr;

const char* cs = R"(

#version 430

layout(binding = 0, std430) coherent buffer HeightfieldDataFloat
{
	float height_buf[];
};

uniform int   grid_sizex;
uniform int   grid_sizey;
uniform float amplitude;
uniform float cell_size;
uniform float tan_threshold_angle;

bool Inside(int x, int y)
{
	if (x < 0 || x >= grid_sizex ||
        y < 0 || y >= grid_sizey) {
		return false;
    }
	return true;
}

int ToIndex1D(int x, int y)
{
	return y * grid_sizex + x;
}

layout(local_size_x = 1024) in;
void main()
{
	uint id = gl_GlobalInvocationID.x;
	if (id >= height_buf.length()) {
        return;
    }

	int i_amplitude = int(amplitude);
	float max_y_diff = 0;
	int   max_idx = -1;
	int y = int(id) / grid_sizex;
	int x = int(id) % grid_sizex;
	for (int k = -1; k <= 1; k += 2)
	{
		for (int l = -1; l <= 1; l += 2)
		{
			if (Inside(x + k, y + l) == false)
				continue;
			int index = ToIndex1D(x + k, y + l);
			float h = float(height_buf[index]);
			float diff = float(height_buf[id] - h);
			if (diff > max_y_diff)
			{
				max_y_diff = diff;
				max_idx = index;
			}
		}
	}
	if (max_idx != -1 && max_y_diff / cell_size > tan_threshold_angle)
	{
		height_buf[id] = height_buf[id] - amplitude;
		height_buf[max_idx] = height_buf[max_idx] + amplitude;
	}
}

)";

}

namespace wm
{
namespace device
{

void ThermalWeathering::Execute()
{
    auto prev_hf = DeviceHelper::GetInputHeight(*this, 0);
    if (!prev_hf) {
        return;
    }

    m_hf = std::make_shared<HeightField>(*prev_hf);

#ifdef THERMAL_WEATHERING_GPU
    auto num = m_hf->Width() * m_hf->Height();
    auto group_sz = EVAL->GetComputeWorkGroupSize();
    int thread_group_count = num / group_sz;
    if (num % group_sz > 0) {
        thread_group_count++;
    }

    for (int i = 0; i < m_iter; ++i) {
        StepGPU(thread_group_count);
    }
#else
    for (int i = 0; i < m_iter; ++i) {
        StepCPU();
    }
#endif // THERMAL_WEATHERING_GPU
}

void ThermalWeathering::Init()
{
#ifdef THERMAL_WEATHERING_GPU
    if (!EVAL)
    {
        auto& rc = ur::Blackboard::Instance()->GetRenderContext();
        EVAL = std::make_shared<EvalGPU>(rc, cs);
    }
#endif // THERMAL_WEATHERING_GPU
}

// from Outerrain
/*
\brief Perform a thermal erosion step with maximum amplitude defined by user. Based on http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.27.8939&rep=rep1&type=pdf.
\param amplitude maximum amount of matter moved from one point to another. Something between [0.05, 0.1] gives plausible results.
*/
void ThermalWeathering::StepCPU()
{
    size_t w = m_hf->Width();
    size_t h = m_hf->Height();

    // todo: 50.0f from wmv renderer
    const float h_scale = 50.0f;
	float cell_dist_x = 1.0f / h_scale;
	for (size_t y = 0; y < h; y++)
	{
		for (size_t x = 0; x < w; x++)
		{
			float max_y_diff = 0.0f;
			int nei_x = -1;
			int nei_y = -1;
			for (int k = -1; k <= 1; k++)
			{
				for (int l = -1; l <= 1; l++)
				{
                    if ((k == 0 && l == 0) ||
                        m_hf->Inside(x + l, y + k) == false) {
                        continue;
                    }
					float h = m_hf->Get(x, y) - m_hf->Get(x + l, y + k);
					if (h > max_y_diff)
					{
						max_y_diff = h;
						nei_x = x + l;
						nei_y = y + k;
					}
				}
			}

			if (nei_x != -1 && max_y_diff / cell_dist_x > m_tan_threshold_angle)
			{
                m_hf->Add(x, y, -m_amplitude);
                m_hf->Add(nei_x, nei_y, m_amplitude);
			}
		}
	}
}

void ThermalWeathering::StepGPU(int thread_group_count)
{
    auto& rc = ur::Blackboard::Instance()->GetRenderContext();

    pt0::ShaderUniforms vals;
    vals.AddVar("grid_sizex",          pt0::RenderVariant(static_cast<int>(m_hf->Width())));
    vals.AddVar("grid_sizey",          pt0::RenderVariant(static_cast<int>(m_hf->Height())));
    vals.AddVar("amplitude",           pt0::RenderVariant(m_amplitude));
    vals.AddVar("tan_threshold_angle", pt0::RenderVariant(m_tan_threshold_angle));
    vals.AddVar("cell_size",           pt0::RenderVariant(1.0f / 50.0f));

    EVAL->RunCS(rc, vals, thread_group_count, *m_hf);
}

}
}