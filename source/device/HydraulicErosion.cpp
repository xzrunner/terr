#include "terraingraph/device/HydraulicErosion.h"
#include "terraingraph/DeviceHelper.h"
#include "terraingraph/Context.h"

#include <heightfield/HeightField.h>
#include <heightfield/Utility.h>

namespace terraingraph
{
namespace device
{

// from Outerrain
void HydraulicErosion::Execute(const std::shared_ptr<dag::Context>& ctx)
{
    auto prev_hf = DeviceHelper::GetInputHeight(*this, 0);
    if (!prev_hf) {
        return;
    }

    m_hf = std::make_shared<hf::HeightField>(*prev_hf);

    auto w = m_hf->Width();
    auto h = m_hf->Height();

	hf::ScalarField2D<double> droplets(w, h, 0);
	hf::ScalarField2D<double> sediments(w, h, 0);

	const float Kd = 0.1f;
	const float Kc = 5.0f;
	const float Ks = 0.3f;

    auto& dev = *std::static_pointer_cast<Context>(ctx)->ur_dev;

	for (size_t x = 0; x < w; x++)
	{
		for (size_t y = 0; y < h; y++)
		{
			std::array<double, 8> waterTransport, neighbourHeightDiff;
			waterTransport.fill(0);
			neighbourHeightDiff.fill(0);
			int lowerVertexCount = 0;
			int index = 0;
			for (int k = -1; k <= 1; k++)
			{
				for (int l = -1; l <= 1; l++)
				{
					if ((k == 0 && l == 0) || m_hf->Inside(x + k, y + l) == false)
						continue;

					neighbourHeightDiff[index] = m_hf->Get(dev, x, y) - m_hf->Get(dev, x + k, y + l);
					if (neighbourHeightDiff[index] <= 0.0f)
					{
						index++;
						continue;
					}

                    double a0 = droplets.Get(x, y);
                    double a1 = droplets.Get(x + k, y + l) + hf::Utility::HeightShortToDouble(m_hf->Get(dev, x + k, y + l));
                    double a2 = a0 + hf::Utility::HeightShortToDouble(m_hf->Get(dev, x, y));
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
					//cout << waterTransport[index] << endl;

					if (waterTransport[index] <= 0.0f)
					{
                        m_hf->Add(x, y, hf::Utility::HeightDoubleToShort(Kd * sediments.Get(x, y)));
						sediments.Set(x, y, (1.0f - Kd) * sediments.Get(x, y));
					}
					else
					{
						droplets.Add(x, y, -waterTransport[index]);
						droplets.Add(x + k, y + l, waterTransport[index]);
						double Cs = Kc * waterTransport[index];
						double sedA = sediments.Get(x, y);

						if (sedA > Cs)
						{
							sediments.Add(x + k, y + l, Cs);
                            m_hf->Add(x, y, hf::Utility::HeightDoubleToShort(Kd * (sedA - Cs)));
							sediments.Set(x, y, (1 - Kd) * (sedA - Cs));
						}
						else
						{
							sediments.Add(x + k, y + l, sedA + Ks * (Cs - sedA));
                            m_hf->Add(x, y, hf::Utility::HeightDoubleToShort(-Kd * (Cs - sedA)));
							sediments.Set(x, y, 0);
						}
					}

					index++;
				}
			}
		}
	}
}

}
}