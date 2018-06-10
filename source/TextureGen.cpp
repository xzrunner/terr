// code from lScape by Adrian Welbourn
// http://www.futurenation.net/glbase/projects.htm

#include "terr/TextureGen.h"

#include <iostream>

namespace
{

typedef float vec3[3];		/* x, y, z */

/* 3D vector vector methods */
#define vec3_cpy(a,b)		{b[0]=a[0];b[1]=a[1];b[2]=a[2];}
#define vec3_neg(a,b)		{b[0]=-a[0];b[1]=-a[1];b[2]=-a[2];}
#define vec3_add(a,b,c)		{c[0]=a[0]+b[0];c[1]=a[1]+b[1];c[2]=a[2]+b[2];}
#define vec3_sub(a,b,c)		{c[0]=a[0]-b[0];c[1]=a[1]-b[1];c[2]=a[2]-b[2];}
#define vec3_mul(a,b,c)		{c[0]=a[0]*b[0];c[1]=a[1]*b[1];c[2]=a[2]*b[2];}
#define vec3_div(a,b,c)		{c[0]=a[0]/b[0];c[1]=a[1]/b[1];c[2]=a[2]/b[2];}
#define vec3_dot(a,b)		(a[0]*b[0]+a[1]*b[1]+a[2]*b[2])
#define vec3_cross(a,b,c)	{c[0] = a[1]*b[2] - a[2]*b[1];\
							 c[1] = a[2]*b[0] - a[0]*b[2];\
							 c[2] = a[0]*b[1] - a[1]*b[0];}

/* 3D Vector Scalar methods */
#define vec3_addS(a,b,c)	{c[0]=a[0]+(b);c[1]=a[1]+(b);c[2]=a[2]+(b);}
#define vec3_subS(a,b,c)	{c[0]=a[0]-(b);c[1]=a[1]-(b);c[2]=a[2]-(b);}
#define vec3_mulS(a,b,c)	{c[0]=a[0]*(b);c[1]=a[1]*(b);c[2]=a[2]*(b);}
#define vec3_divS(a,b,c)	{float d=1.0f/(b); c[0]=a[0]*d;c[1]=a[1]*d;c[2]=a[2]*d;}

/* 3D Vector methods */
#define vec3_getLen(a)		(float)sqrt(a[0]*a[0]+a[1]*a[1]+a[2]*a[2])
#define vec3_norm(a)		{float b=1.0f/(float)sqrt(a[0]*a[0]+a[1]*a[1]+a[2]*a[2]);\
							 a[0]=a[0]*b;a[1]=a[1]*b;a[2]=a[2]*b;}

/*
shadowMapPut()

This is a helper function for drawing into the shadow map. This function
is used when casting a ray from a heightmap point. If the height of the
heightmap at point x, y is less than the height z of the ray at point x, y
then a full darkness shadow pixel is drawn.
*/
void ShadowMapPut(uint8_t* shadow_map, const uint8_t* height_map,
	              int size, int x, int y, float z)
{
	/* Assume size is a power of two */
	x = x & (size - 1);
	y = y & (size - 1);
//	if (height_map[x + y * (size + 1)] <= z)
	if (height_map[x + (y + 1) * size] <= z)
		shadow_map[x + y * size] = 255;
}

/*
  calcBumps()

  This calculates the bump map that is used to apply some noise to the normal
  map. This isn't a bump map in the usual graphics sense, its just random
  noise that is used to vary the texture applied to flat areas of the map as
  if the terrain were more bumpy.
*/
uint8_t* CalcBumps(int size)
{
	uint8_t* bump_map = new uint8_t[size * size];
	if (!bump_map) {
		std::cerr << "CalcBumps alloc fail.\n";
		return bump_map;
	}
	memset(bump_map, 0, size * size);

	for (int j = 0; j < size; j++) {
		for (int i = 0; i < size; i++) {
			float p = (float)rand() / (float)RAND_MAX;
			if (p > 1.0f)
				p = 1.0f;
			bump_map[j*size + i] = (uint8_t)(255.0f * p);
		}
	}

	return bump_map;
}

/*
  blurLightMap()

  Applies a simple blurring filter to the light map. The blurring filter is
  first weighted according to the light direction.
*/
void BlurLightMap(uint8_t* light_map, int size, vec3 dir)
{
	int blurMap[9] = { 64,  64,  64,
					   64, 255,  64,
					   64,  64,  64 };
	int X, Y, i, divisor = 0;

	uint8_t* lightMap2 = new uint8_t[size * size];
	if (!lightMap2) {
		std::cerr << "BlurLightMap alloc fail.\n";
		return;
	}
	memset(lightMap2, 0, size * size);

	if (dir[0] > 0.6) {
		blurMap[2] = blurMap[5] = blurMap[8] = 128;
	}
	else if (dir[0] < 0.6) {
		blurMap[0] = blurMap[3] = blurMap[6] = 128;
	}
	if (dir[1] > 0.6) {
		blurMap[0] = blurMap[1] = blurMap[2] = 128;
	}
	else if (dir[1] < 0.6) {
		blurMap[6] = blurMap[7] = blurMap[8] = 128;
	}

	for (i = 0; i < 9; i++)
		divisor += blurMap[i];

	/* For each heightmap vertex */
	for (Y = 0; Y < size; Y++) {
		for (X = 0; X < size; X++) {
			int x, y, accum = 0;
			for (y = 0; y < 3; y++) {
				for (x = 0; x < 3; x++) {
					accum += blurMap[x + 3*y]
						* terr::TextureGen::LightMapGet(light_map, size, X+x-1, Y+y-1);
				}
			}
			accum /= divisor;
			if (accum > 255)
				accum = 255;
			terr::TextureGen::LightMapPut(lightMap2, size, X, Y, (uint8_t)accum);
		}
	}
	memcpy(light_map, lightMap2, size * size);
	delete[] lightMap2;
}

}

namespace terr
{

/*
lightMapPut()

Put a pixel value into the light map.
*/
void TextureGen::LightMapPut(uint8_t* light_map, int size, int x, int y, uint8_t val)
{
	/* Assume size is a power of two */
	x = x & (size - 1);
	y = y & (size - 1);
	light_map[x + y * size] = val;
}

/*
  lightMapGet()

  Get a pixel value from the light map.
*/
uint8_t TextureGen::LightMapGet(const uint8_t* light_map, int size, int x, int y)
{
	/* Assume size is a power of two */
	x = x & (size - 1);
	y = y & (size - 1);
	return light_map[x + y*size];
}

const uint8_t* TextureGen::NormalMapGet(const uint8_t* normal_map, int size, int x, int y)
{
	/* Assume size is a power of two */
	x = x & (size - 1);
	y = y & (size - 1);
	return &normal_map[3 * (x + y * size)];
}

/*
  calcNormals()

  Calculate a normal for each heightmap vertex by calculating a normal for
  each of the eight triangles sharing the vertex and averaging. The three
  components of the normal are scaled to the range +-127 and stored as signed
  char values.

  If rand_scale and frequency are both greater than zero, a bump map (above)
  is generated with one point for every frequency points of the normal map,
  e.g. if the normal map is n x n and frequency is 4, the bump map will be
  n/4 x n/4 and will then be interpolated across the normal map, smoothing the
  noise out. The cutoff parameter is the lower limit to normal.z above which
  noise is applied. This allows noise to only be applied to the flattest
  parts of the map if required.

  The bump map is deleted on completion.
*/
uint8_t* TextureGen::CalcNormals(const uint8_t* height_map, int size,
	                             const float scale[3], float rand_scale,
	                             float cutoff, int frequency)
{
	uint8_t* bump_map = NULL;
	int bump_map_size;
	float scale_bump = rand_scale / 255.0f;
	float inv_freq = 1.0f;
	uint8_t* normal_map;
	int X, Y, i, j;
	vec3 normal, temp, temp0, temp1;

	/*
	 * Eight vectors connecting a point in the
	 * heightmap to each of its neighbours.
	 */
	float v[24] = { -1, -1,  0,
					 0, -1,  0,
					 1, -1,  0,
					 1,  0,  0,
					 1,  1,  0,
					 0,  1,  0,
					-1,  1,  0,
					-1,  0,  0 };

	printf("Calculating normal map");
	fflush(stdout);

	/* Calculate the bumpmap */
	if (rand_scale > 0.0f && frequency > 0) {
		if (frequency > size)
			frequency = size;
		bump_map_size = size / frequency;
		bump_map = CalcBumps(bump_map_size);
		inv_freq = 1.0f / frequency;
	}

	/* Allocate space for the normal map */
	normal_map = new uint8_t[3 * size * size];
	if (!normal_map) {
		std::cerr << "CalcNormals alloc fail.\n";
		if (bump_map) {
			delete[] bump_map;
		}
		return normal_map;
	}
	memset(normal_map, 0, 3 * size * size);

	/* For each point in the heightmap */
	for (Y = 0; Y < size; Y++) {

		/* Show progress */
		if (!(Y & 0x3F)) {
			printf(".");
			fflush(stdout);
		}

		for (X = 0; X < size; X++) {

			/* Get the eight vectors connecting
			   this point to its neighbours */
			for (i = 0; i < 8; i++) {
				int x, y, z;

				/* Assume size is a power of two */
				x = (X + (int)(v[3*i + 0])) & (size - 1);
				y = (Y + (int)(v[3*i + 1])) & (size - 1);
//				z = (int)height_map[x + y*(size + 1)]
//					- (int)height_map[X + Y*(size + 1)];
				z = (int)height_map[x + (Y + 1) * size]
					- (int)height_map[X + (Y + 1) * size];
				v[3*i + 2] = (float)z;
			}

			/* Calculate the eight cross products and average */
			for (i = 0; i < 3; i++)
				normal[i] = 0;
			for (i = 0, j = 1; i < 8; i++, j++) {
				if (j >= 8)
					j = 0;
				vec3_mul((&v[i*3]), scale, temp0);
				vec3_mul((&v[j*3]), scale, temp1);
				vec3_cross(temp0, temp1, temp);
				/* Make sure the normal points up */
				if (temp[2] < 0.0f)
					vec3_mulS(temp, -1.0f, temp);
				vec3_add(normal, temp, normal);
			}
			vec3_norm(normal);

			/* If we have a bump map and normal.z is above
			   the threshold, apply some noise */
			if (normal[2] >= cutoff && bump_map) {
				float fbX = (float)X * inv_freq;
				float fbY = (float)Y * inv_freq;
				int bX0 = (int)floor(fbX);
				int bX1 = (int)ceil(fbX);
				int bY0 = (int)floor(fbY);
				int bY1 = (int)ceil(fbY);
				float b;

				if (bX0 == bX1 && bY0 == bY1) {
					/* Heightmap point corresponds to an
					   exact bump map point */
					b = LightMapGet(bump_map, bump_map_size, bX0, bY0);
				}
				else {
					/* Heightmap point falls between bump
					   map points, interpolate */
					float scale;
					float b00 = LightMapGet(bump_map, bump_map_size, bX0, bY0);
					float b01 = LightMapGet(bump_map, bump_map_size, bX1, bY0);
					float b10 = LightMapGet(bump_map, bump_map_size, bX0, bY1);
					float b11 = LightMapGet(bump_map, bump_map_size, bX1, bY1);
					float b0 = b00;
					float b1 = b10;

					if (bX0 != bX1) {
						/* Lerp x */
						scale = (float)bX1 - fbX;
						b0 *= scale;
						b1 *= scale;
						scale = fbX - (float)bX0;
						b0 += scale * b01;
						b1 += scale * b11;
					}

					b = b0;
					if (bY0 != bY1) {
						/* Lerp y */
						scale = (float)bY1 - fbY;
						b *= scale;
						scale = fbY - (float)bY0;
						b += scale * b1;
					}
				}

				/* Apply noise and re-normalize */
				b *= scale_bump;
				normal[2] -= b;

/*
  Assuming z is near 1 and x, y and noise b are small compared to z,
  we have to add something to x & y otherwise when we renormalize,
  z returns to near its previous value. The something added to x & y
  should tend to keep the normal's length unchanged so that when we
  renormalize, z retains its adjusted value.

  Since

    x^2 + y^2 + z'^2  approx=  x^2 + y^2 + z^2 - 2b

  if we add sqrt(b) to x and y we get

    x'^2 + y'^2 + z'^2  approx=  x^2 + b + y^2 + b + z^2 - 2b
			      =  x^2 + y^2 + z^2

  which is what we want.
*/
				b = sqrt(b);
				normal[0] += b;
				normal[1] += b;
				if (normal[2] < 0.0f)
					normal[2] = 0.0f;
				vec3_norm(normal);
			}

			/* Save the normal in the normal map */
			normal_map[3*(X + Y*size) + 0] = (char)(127.0f * normal[0]);
			normal_map[3*(X + Y*size) + 1] = (char)(127.0f * normal[1]);
			normal_map[3*(X + Y*size) + 2] = (char)(127.0f * normal[2]);
		}
	}

	/* Delete the bump map */
	if (bump_map) {
		delete[] bump_map;
	}

	printf("\n");
	fflush(stdout);

	return normal_map;
}

/*
  calcShadows()

  For each point in the heightmap we cast a ray in the light direction. Any
  points intersected in x-y which are below the ray in z are deemed to be in
  shadow.
*/
uint8_t* TextureGen::CalcShadows(const uint8_t* height_map, int size,
	                             const float scale[3], const float dir[3])
{
	const float scale_normal = 1.0f / 127.0f;
	uint8_t* shadow_map;
	vec3 L;
	int X, Y;

	printf("Calculating shadow map");
	fflush(stdout);

	/* Allocate space for the shadow map */
	shadow_map = new uint8_t[size * size];
	if (!shadow_map) {
		std::cerr << "CalcShadows alloc fail.\n";
		return shadow_map;
	}
	memset(shadow_map, 0, size * size);

	/* Make sure the light source is normalised */
	vec3_cpy(dir, L);
	vec3_div(L, scale, L);
	vec3_norm(L);
	if (L[2] == 0.0f) {
		/* Pathological case */
		std::cerr << "CalcShadows: light vector horizontal.\n";
		return shadow_map;
	}

	/* For each heightmap vertex */
	for (Y = 0; Y < size - 1; Y++) {

		/* Show progress */
		if (!(Y & 0x3F)) {
			printf(".");
			fflush(stdout);
		}

		for (X = 0; X < size; X++) {
			float z;

			/* If vertex already in shadow ignore it */
			if (shadow_map[X + Y*size])
				continue;

			/* Step along a line through the vertex in the direction of L */

//			z = (float)height_map[X + Y*(size + 1)];
			z = (float)height_map[X + (Y + 1) * size];
			if (fabs(L[0]) < fabs(L[1])) {
				float incx = L[0] / L[1];
				float incz = L[2] / L[1];
				int y, incy = 1;
				float x;
				if (L[1] < 0) {
					incx = -incx; incy = -incy, incz = -incz;
				}
				x = X + incx;
				z += incz;
				for (y = Y + incy; 1; x += incx, y += incy, z += incz) {
					if (z < 0.0f)
						break;
					ShadowMapPut(shadow_map, height_map, size, (int)x, y, z);
				}
			}
			else {
				float incy = L[1] / L[0];
				float incz = L[2] / L[0];
				int x, incx = 1;
				float y;
				if (L[0] < 0) {
					incx = -incx; incy = -incy, incz = -incz;
				}
				y = Y + incy;
				z += incz;
				for (x = X + incx; 1; x += incx, y += incy, z += incz) {
					if (z < 0.0f)
						break;
					ShadowMapPut(shadow_map, height_map, size, x, (int)y, z);
				}
			}
		}
	}

	printf("\n");
	fflush(stdout);

	return shadow_map;
}

uint8_t* TextureGen::CalcLighting(const uint8_t* normal_map, const uint8_t* shadow_map,
	                              int size, const float scale[3], float amb, float diff, const float dir[3])
{
	const float scaleNormal = 1.0f / 127.0f;
	const float scaleShadow = 1.0f / 255.0f;
	int X, Y;
	vec3 L, normal;

	printf("Calculating light map");
	fflush(stdout);

	/* Allocate space for the light map */
	uint8_t* lightMap = new uint8_t[size * size];
	if (!lightMap) {
		std::cerr << "TextureGen alloc fail.\n";
		return lightMap;
	}
	memset(lightMap, 0, size * size);

	/* Make sure the light source is normalised */
	vec3_cpy(dir, L);
	vec3_div(L, scale, L);
	vec3_norm(L);
	vec3_mulS(L, -1.0f, L);

	/* For each point */
	for (Y = 0; Y < size; Y++) {

		/* Show progress */
		if (!(Y & 0x3F)) {
			printf(".");
			fflush(stdout);
		}

		for (X = 0; X < size; X++) {
			float i, s, d;

			/* Get the dot product with the light source direction */
			normal[0] = normal_map[3*(X + Y*size) + 0];
			normal[1] = normal_map[3*(X + Y*size) + 1];
			normal[2] = normal_map[3*(X + Y*size) + 2];
			vec3_mulS(normal, scaleNormal, normal);
			d = vec3_dot(L, normal);
			if (d < 0.0f)
				d = 0.0f;

			/* Calculate illumination model (ambient plus diffuse) */
			s = 1.0f;

			if (shadow_map)
				s = 1.0f - scaleShadow * shadow_map[X + Y*size];
			i = amb + s * d * diff;
			if (i < 0.0f)
				i = 0.0f;
			else if (i > 1.0f)
				i = 1.0f;

			/* Save in light map */
			lightMap[X + Y*size] = (uint8_t)(i * 255.0f);
		}
	}

	/* Blur the light map */
	printf("blurring");
	fflush(stdout);
	BlurLightMap(lightMap, size, L);
	BlurLightMap(lightMap, size, L);

	printf("\n");
	fflush(stdout);

	return lightMap;
}

}