#include "terraingraph/device/PlasmaFractal.h"
#include "terraingraph/Utility.h"

#include <heightfield/HeightField.h>
#include <heightfield/Utility.h>

namespace terraingraph
{
namespace device
{

void PlasmaFractal::Execute(const std::shared_ptr<dag::Context>& ctx)
{
    m_hf = std::make_shared<hf::HeightField>(m_width, m_height);

    MakePlasma(m_width, m_height);
}

void PlasmaFractal::MakePlasma(size_t _width, size_t _height)
{
    if (m_roughness < 0) {
        m_roughness *= -1;
    }

    int rect_size_x = _width;
    int rect_size_y = _height;
	float height = (float)hf::Utility::HeightFloatToShort(1.0f) * 2;
	float height_reducer = (float)pow(2, -1 * m_roughness);

	//allocate the memory for our height data

	std::vector<int32_t> temp_buf(_width * _height, 0);

	//set the first value in the height field
	temp_buf[0] = 0;

	//being the displacement process
	while (rect_size_x > 0 && rect_size_y > 0)
	{
		/*Diamond step -

		Find the values at the center of the retangles by averaging the values at
		the corners and adding a random offset:


		a.....b
		.     .
		.  e  .
		.     .
		c.....d

		e  = (a+b+c+d)/4 + random

		In the code below:
		a = (i,j)
		b = (ni,j)
		c = (i,nj)
		d = (ni,nj)
		e = (mi,mj)   */
		for (int i = 0; i < static_cast<int>(_width); i += rect_size_x)
		{
			for (int j = 0; j < static_cast<int>(_height); j += rect_size_y)
			{
                int ni = (i + rect_size_x) % _width;
                int nj = (j + rect_size_y) % _height;

                int mi = (i + rect_size_x / 2);
                int mj = (j + rect_size_y / 2);

				temp_buf[mi + mj * _width] = static_cast<int32_t>((temp_buf[i + j * _width] + temp_buf[ni + j * _width] + temp_buf[i + nj * _width] + temp_buf[ni + nj * _width]) / 4 + Utility::RangedRandom(-height / 2, height / 2));
			}
		}

		/*Square step -

		Find the values on the left and top sides of each rectangle
		The right and bottom sides are the left and top sides of the neighboring rectangles,
		so we don't need to calculate them

		The height m_heightData.m_ucpData wraps, so we're never left hanging.  The right side of the last
		rectangle in a row is the left side of the first rectangle in the row.  The bottom
		side of the last rectangle in a column is the top side of the first rectangle in
		the column

		.......
		.     .
		.     .
		.  d  .
		.     .
		.     .
		......a..g..b
		.     .     .
		.     .     .
		.  e  h  f  .
		.     .     .
		.     .     .
		......c......

		g = (d+f+a+b)/4 + random
		h = (a+c+e+f)/4 + random

		In the code below:
		a = (i,j)
		b = (ni,j)
		c = (i,nj)
		d = (mi,pmj)
		e = (pmi,mj)
		f = (mi,mj)
		g = (mi,j)
		h = (i,mj)*/
		for (int i = 0; i < static_cast<int>(_width); i += rect_size_x)
		{
			for (int j = 0; j < static_cast<int>(_height); j += rect_size_y)
			{

                int ni = (i + rect_size_x) % _width;
                int nj = (j + rect_size_y) % _height;

                int mi = (i + rect_size_x / 2);
                int mj = (j + rect_size_y / 2);

                int pmi = (i - rect_size_x / 2 + _width) % _width;
                int pmj = (j - rect_size_y / 2 + _height) % _height;

				//Calculate the square value for the top side of the rectangle
				temp_buf[mi + j * _width] = static_cast<int32_t>((temp_buf[i + j * _width] +
					temp_buf[ni + j * _width] +
					temp_buf[mi + pmj * _width] +
					temp_buf[mi + mj * _width]) / 4 +
                    Utility::RangedRandom(-height / 2, height / 2));

				//Calculate the square value for the left side of the rectangle
				temp_buf[i + mj * _width] = static_cast<int32_t>((temp_buf[i + j * _width] +
					temp_buf[i + nj * _width] +
					temp_buf[pmi + mj * _width] +
					temp_buf[mi + mj * _width]) / 4 +
                    Utility::RangedRandom(-height / 2, height / 2));
			}
		}

		//reduce the rectangle size by two to prepare for the next
		//displacement stage
		rect_size_x /= 2;
        rect_size_y /= 2;

		//reduce the height by the height reducer
		height *= height_reducer;
	}

    m_hf->SetValues(temp_buf);
}

}
}