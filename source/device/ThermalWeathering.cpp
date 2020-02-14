#include "wm/device/ThermalWeathering.h"
#include "wm/DeviceHelper.h"
#include "wm/HeightField.h"

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

    for (int i = 0; i < m_iter; ++i) {
        Step();
    }
}

/*
\brief Perform a thermal erosion step with maximum amplitude defined by user. Based on http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.27.8939&rep=rep1&type=pdf.
\param amplitude maximum amount of matter moved from one point to another. Something between [0.05, 0.1] gives plausible results.
*/
void ThermalWeathering::Step()
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

}
}