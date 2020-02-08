#include "wm/device/FaultFractal.h"
#include "wm/Context.h"
#include "wm/Utility.h"
#include "wm/HeightField.h"

namespace wm
{
namespace device
{

void FaultFractal::Execute(const Context& ctx)
{
    m_hf = std::make_shared<HeightField>(ctx.size, ctx.size);

    MakeFault(ctx.size);
}

void FaultFractal::MakeFault(size_t size)
{
	//allocate the memory for our height data
    std::vector<float> temp_buf(size * size, 0);

	for (int curr_iter = 0; curr_iter < m_iter; curr_iter++)
	{
		//calculate the height range (linear interpolation from iMaxDelta to
		//iMinDelta) for this fault-pass
		int height = m_max_delta - ((m_max_delta - m_min_delta) * curr_iter) / m_iter;

		//pick two points at random from the entire height map
		int rand_x1 = rand() % size;
        int rand_z1 = rand() % size;
        int rand_x2 = 0, rand_z2 = 0;

		//check to make sure that the points are not the same
		do
		{
            rand_x2 = rand() % size;
            rand_z2 = rand() % size;
		} while (rand_x2 == rand_x1 && rand_z2 == rand_z1);


		//dir_x1, dir_z1 is a vector going the same direction as the line
        int dir_x1 = rand_x2 - rand_x1;
        int dir_z1 = rand_z2 - rand_z1;

		for (size_t z = 0; z < size; ++z)
		{
			for (size_t x = 0; x < size; ++x)
			{
				//dir_x2, dir_z2 is a vector from rand_x1, rand_z1 to the current point (in the loop)
                int dir_x2 = x - rand_x1;
                int dir_z2 = z - rand_z1;

				//if the result of ( dir_x2*dir_z1 - dir_x1*dir_z2 ) is "up" (above 0),
				//then raise this point by height
				if ((dir_x2*dir_z1 - dir_x1 * dir_z2)>0)
					temp_buf[(z*size) + x] += (float)height;
			}
		}

		//erode terrain
		FilterHeightField(size, temp_buf, m_filter);
	}

    m_hf->Fill(temp_buf);
    m_hf->Normalize();
}

// Apply the erosion filter to an individual band of height values
void FaultFractal::FilterHeightBand(float* band, int stride, int count, float filter)
{
	float v = band[0];
	int j = stride;

	//go through the height band and apply the erosion filter
	for (int i = 0; i < count - 1; ++i)
	{
		band[j] = filter * v + (1 - filter)*band[j];

		v = band[j];
		j += stride;
	}
}

// Apply the erosion filter to an entire buffer of height values
void FaultFractal::FilterHeightField(int size, std::vector<float>& height_data, float filter)
{
	//erode left to right
    for (int i = 0; i < size; ++i) {
        FilterHeightBand(&height_data[size*i], 1, size, filter);
    }

	//erode right to left
    for (int i = 0; i < size; ++i) {
        FilterHeightBand(&height_data[size*i + size - 1], -1, size, filter);
    }

	//erode top to bottom
    for (int i = 0; i < size; ++i) {
        FilterHeightBand(&height_data[i], size, size, filter);
    }

	//erode from bottom to top
    for (int i = 0; i < size; ++i) {
        FilterHeightBand(&height_data[size*(size - 1) + i], -size, size, filter);
    }
}

}
}