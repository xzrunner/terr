#include "wm/device/PlasmaFractal.h"
#include "wm/Context.h"
#include "wm/Utility.h"
#include "wm/HeightField.h"

namespace wm
{
namespace device
{

void PlasmaFractal::Execute(const Context& ctx)
{
    m_hf = std::make_shared<HeightField>(ctx.size, ctx.size);

    MakePlasma(ctx.size);
}

void PlasmaFractal::MakePlasma(size_t size)
{
    if (m_roughness < 0) {
        m_roughness *= -1;
    }

    int rect_size = size;
	float height = (float)rect_size / 2;
	float height_reducer = (float)pow(2, -1 * m_roughness);

	//allocate the memory for our height data

	std::vector<float> temp_buf(size * size, 0);

	//set the first value in the height field
	temp_buf[0] = 0.0f;

	//being the displacement process
	while (rect_size > 0)
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
		for (int i = 0; i < size; i += rect_size)
		{
			for (int j = 0; j < size; j += rect_size)
			{
                int ni = (i + rect_size) % size;
                int nj = (j + rect_size) % size;

                int mi = (i + rect_size / 2);
                int mj = (j + rect_size / 2);

				temp_buf[mi + mj * size] = (float)((temp_buf[i + j * size] + temp_buf[ni + j * size] + temp_buf[i + nj * size] + temp_buf[ni + nj * size]) / 4 + Utility::RangedRandom(-height / 2, height / 2));
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
		for (int i = 0; i<size; i += rect_size)
		{
			for (int j = 0; j<size; j += rect_size)
			{

                int ni = (i + rect_size) % size;
                int nj = (j + rect_size) % size;

                int mi = (i + rect_size / 2);
                int mj = (j + rect_size / 2);

                int pmi = (i - rect_size / 2 + size) % size;
                int pmj = (j - rect_size / 2 + size) % size;

				//Calculate the square value for the top side of the rectangle
				temp_buf[mi + j * size] = (float)((temp_buf[i + j * size] +
					temp_buf[ni + j * size] +
					temp_buf[mi + pmj * size] +
					temp_buf[mi + mj * size]) / 4 +
                    Utility::RangedRandom(-height / 2, height / 2));

				//Calculate the square value for the left side of the rectangle
				temp_buf[i + mj * size] = (float)((temp_buf[i + j * size] +
					temp_buf[i + nj * size] +
					temp_buf[pmi + mj * size] +
					temp_buf[mi + mj * size]) / 4 +
                    Utility::RangedRandom(-height / 2, height / 2));
			}
		}

		//reduce the rectangle size by two to prepare for the next
		//displacement stage
		rect_size /= 2;

		//reduce the height by the height reducer
		height *= height_reducer;
	}

    m_hf->Fill(temp_buf);
    m_hf->Normalize();
}

}
}