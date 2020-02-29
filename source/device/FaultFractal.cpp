#include "terraingraph/device/FaultFractal.h"
#include "terraingraph/Utility.h"
#include "terraingraph/HeightField.h"

namespace terraingraph
{
namespace device
{

void FaultFractal::Execute()
{
    m_hf = std::make_shared<HeightField>(m_width, m_height);

    MakeFault(m_width, m_height);
}

void FaultFractal::MakeFault(size_t _width, size_t _height)
{
	//allocate the memory for our height data
    std::vector<float> temp_buf(_width * _height, 0);

	for (int curr_iter = 0; curr_iter < m_iter; curr_iter++)
	{
		//calculate the height range (linear interpolation from iMaxDelta to
		//iMinDelta) for this fault-pass
		int height = m_max_delta - ((m_max_delta - m_min_delta) * curr_iter) / m_iter;

		//pick two points at random from the entire height map
		int rand_x1 = rand() % _width;
        int rand_z1 = rand() % _height;
        int rand_x2 = 0, rand_z2 = 0;

		//check to make sure that the points are not the same
		do
		{
            rand_x2 = rand() % _width;
            rand_z2 = rand() % _height;
		} while (rand_x2 == rand_x1 && rand_z2 == rand_z1);


		//dir_x1, dir_z1 is a vector going the same direction as the line
        int dir_x1 = rand_x2 - rand_x1;
        int dir_z1 = rand_z2 - rand_z1;

		for (size_t z = 0; z < _height; ++z)
		{
			for (size_t x = 0; x < _width; ++x)
			{
				//dir_x2, dir_z2 is a vector from rand_x1, rand_z1 to the current point (in the loop)
                int dir_x2 = x - rand_x1;
                int dir_z2 = z - rand_z1;

				//if the result of ( dir_x2*dir_z1 - dir_x1*dir_z2 ) is "up" (above 0),
				//then raise this point by height
				if ((dir_x2*dir_z1 - dir_x1 * dir_z2)>0)
					temp_buf[(z*_width) + x] += (float)height;
			}
		}

		//erode terrain
		FilterHeightField(_width, _height, temp_buf, m_filter);
	}

    m_hf->SetValues(temp_buf);
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
void FaultFractal::FilterHeightField(size_t width, size_t height, std::vector<float>& height_data, float filter)
{
	//erode left to right
    for (int i = 0; i < height; ++i) {
        FilterHeightBand(&height_data[width*i], 1, width, filter);
    }

	//erode right to left
    for (int i = 0; i < height; ++i) {
        FilterHeightBand(&height_data[width*i + width - 1], -1, width, filter);
    }

	//erode top to bottom
    for (int i = 0; i < width; ++i) {
        FilterHeightBand(&height_data[i], width, height, filter);
    }

	//erode from bottom to top
    for (int i = 0; i < width; ++i) {
        FilterHeightBand(&height_data[width*(height - 1) + i], -width, height, filter);
    }
}

}
}