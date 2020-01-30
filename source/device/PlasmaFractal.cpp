#include "terr/device/PlasmaFractal.h"
#include "terr/Context.h"
#include "terr/Utility.h"
#include "terr/HeightField.h"

namespace terr
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
	float fHeight, fHeightReducer;
	int iRectSize = size;
	int ni, nj;
	int mi, mj;
	int pmi, pmj;
	int i, j;
	int x, z;

    if (m_roughness < 0) {
        m_roughness *= -1;
    }

	fHeight = (float)iRectSize / 2;
	fHeightReducer = (float)pow(2, -1 * m_roughness);

	//allocate the memory for our height data

	std::vector<float> fTempBuffer(size * size, 0);

	//set the first value in the height field
	fTempBuffer[0] = 0.0f;

	//being the displacement process
	while (iRectSize>0)
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
		for (i = 0; i<size; i += iRectSize)
		{
			for (j = 0; j<size; j += iRectSize)
			{
				ni = (i + iRectSize) % size;
				nj = (j + iRectSize) % size;

				mi = (i + iRectSize / 2);
				mj = (j + iRectSize / 2);

				fTempBuffer[mi + mj * size] = (float)((fTempBuffer[i + j * size] + fTempBuffer[ni + j * size] + fTempBuffer[i + nj * size] + fTempBuffer[ni + nj * size]) / 4 + Utility::RangedRandom(-fHeight / 2, fHeight / 2));
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
		for (i = 0; i<size; i += iRectSize)
		{
			for (j = 0; j<size; j += iRectSize)
			{

				ni = (i + iRectSize) % size;
				nj = (j + iRectSize) % size;

				mi = (i + iRectSize / 2);
				mj = (j + iRectSize / 2);

				pmi = (i - iRectSize / 2 + size) % size;
				pmj = (j - iRectSize / 2 + size) % size;

				//Calculate the square value for the top side of the rectangle
				fTempBuffer[mi + j * size] = (float)((fTempBuffer[i + j * size] +
					fTempBuffer[ni + j * size] +
					fTempBuffer[mi + pmj * size] +
					fTempBuffer[mi + mj * size]) / 4 +
                    Utility::RangedRandom(-fHeight / 2, fHeight / 2));

				//Calculate the square value for the left side of the rectangle
				fTempBuffer[i + mj * size] = (float)((fTempBuffer[i + j * size] +
					fTempBuffer[i + nj * size] +
					fTempBuffer[pmi + mj * size] +
					fTempBuffer[mi + mj * size]) / 4 +
                    Utility::RangedRandom(-fHeight / 2, fHeight / 2));
			}
		}

		//reduce the rectangle size by two to prepare for the next
		//displacement stage
		iRectSize /= 2;

		//reduce the height by the height reducer
		fHeight *= fHeightReducer;
	}

	//normalize the terrain for our purposes
    NormalizeTerrain(fTempBuffer);

    m_hf->Fill(fTempBuffer);
		height *= height_reducer;
	}

}

}
}