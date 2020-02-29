#include "terraingraph/device/HydraulicErosion.h"
#include "terraingraph/DeviceHelper.h"
#include "terraingraph/HeightField.h"

namespace terraingraph
{
namespace device
{

// from Outerrain
void HydraulicErosion::Execute()
{
    auto prev_hf = DeviceHelper::GetInputHeight(*this, 0);
    if (!prev_hf) {
        return;
    }

    m_hf = std::make_shared<HeightField>(*prev_hf);

    auto w = m_hf->Width();
    auto h = m_hf->Height();

	ScalarField2D droplets(w, h, 1.0f);
	ScalarField2D sediments(w, h, 1.0f);

	const float Kd = 0.1f;
	const float Kc = 5.0f;
	const float Ks = 0.3f;

	for (size_t x = 0; x < w; x++)
	{
		for (size_t y = 0; y < h; y++)
		{
			std::array<float, 8> waterTransport, neighbourHeightDiff;
			waterTransport.fill(0);
			neighbourHeightDiff.fill(0.0f);
			int lowerVertexCount = 0;
			int index = 0;
			for (int k = -1; k <= 1; k++)
			{
				for (int l = -1; l <= 1; l++)
				{
					if ((k == 0 && l == 0) || m_hf->Inside(x + k, y + l) == false)
						continue;

					neighbourHeightDiff[index] = m_hf->Get(x, y) - m_hf->Get(x + k, y + l);
					if (neighbourHeightDiff[index] <= 0.0f)
					{
						index++;
						continue;
					}

					float a0 = droplets.Get(x, y);
					float a1 = droplets.Get(x + k, y + l) + m_hf->Get(x + k, y + l);
					float a2 = a0 + m_hf->Get(x, y);
					waterTransport[index] = std::min(a0, a2 - a1);
					lowerVertexCount++;
					index++;
				}
			}

			index = 0;
			for (int k = -1; k <= 1; k++)
			{
				for (int l = -1; l <= 1; l++)
				{
					if ((k == 0 && l == 0) || m_hf->Inside(x + k, y + l) == false)
						continue;
					if (neighbourHeightDiff[index] <= 0.0f)
					{
						index++;
						continue;
					}

					// Bug here for sure
					// Remake implementation based on http://hpcg.purdue.edu/bbenes/papers/Benes02WSCG.pdf
					// Instead of Musgrave, which is not very precise and misses some details.
					waterTransport[index] = waterTransport[index] * neighbourHeightDiff[index] / lowerVertexCount;
					waterTransport[index] = std::min(std::max(waterTransport[index], 0.0f), 1.0f);
					//cout << waterTransport[index] << endl;

					if (waterTransport[index] <= 0.0f)
					{
                        m_hf->Add(x, y, Kd * sediments.Get(x, y));
						sediments.Set(x, y, (1.0f - Kd) * sediments.Get(x, y));
					}
					else
					{
						droplets.Add(x, y, -waterTransport[index]);
						droplets.Add(x + k, y + l, waterTransport[index]);
						float Cs = Kc * waterTransport[index];
						float sedA = sediments.Get(x, y);

						if (sedA > Cs)
						{
							sediments.Add(x + k, y + l, Cs);
                            m_hf->Add(x, y, Kd * (sedA - Cs));
							sediments.Set(x, y, (1 - Kd) * (sedA - Cs));
						}
						else
						{
							sediments.Add(x + k, y + l, sedA + Ks * (Cs - sedA));
                            m_hf->Add(x, y, - Kd * (Cs - sedA));
							sediments.Set(x, y, 0.0f);
						}
					}

					index++;
				}
			}
		}
	}

    m_hf->Normalize();
}

}
}