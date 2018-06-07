// code from http://vterrain.org/
// An implementation a terrain rendering engine
// based on the ideas and input of Seumas McNally.

#include "terr/SplitOnlyROAM.h"
#include "terr/BinTriPool.h"

#include <cmath>

#define ADAPTION_SPEED			.00003f
#define QUALITYCONSTANT_MIN		0.002f

namespace
{

/**
* Compute base-2 logarithm of an integer.
* There's probably an even simpler way to do this with sneaky
* logical ops or without a loop, but this works.
*/
int log2(int n)
{
	int temp = n, i;
	for (i = 0; temp > 1; i++)
		temp >>= 1;
	return i;
}

}

namespace terr
{

SplitOnlyROAM::SplitOnlyROAM(int size, BinTriPool& pool)
	: m_size(size)
	, m_pool(pool)
	, m_variance(nullptr)
	, m_hypo_len(nullptr)
{
	int n = log2(m_size - 1);
	// the triangle bintree will have (2n + 2) levels
	m_levels = 2 * n + 2;
	// however, we don't need to store variance for the bottom-most nodes
	// of the tree, so use one less level, and add 1 for 1-based numbering
	m_used_nodes = (1 << (m_levels - 1));

	m_split_cutoff = (1 << (m_levels - 2));

	m_quality_constant = 0.1f;	// safe initial value
//	m_drawn_tris = -1;
}

SplitOnlyROAM::~SplitOnlyROAM()
{
	delete[] m_variance;
	delete[] m_hypo_len;
}

void SplitOnlyROAM::Reset()
{
	m_pool.Reset();

	m_nw_tri = m_pool.Alloc();
	m_se_tri = m_pool.Alloc();

	m_nw_tri->left_child     = nullptr;
	m_nw_tri->right_child    = nullptr;
	m_nw_tri->left_neighbor  = nullptr;
	m_nw_tri->right_neighbor = nullptr;
	m_nw_tri->base_neighbor  = m_se_tri;

	m_se_tri->left_child     = nullptr;
	m_se_tri->right_child    = nullptr;
	m_se_tri->left_neighbor  = nullptr;
	m_se_tri->right_neighbor = nullptr;
	m_se_tri->base_neighbor  = m_nw_tri;
}

void SplitOnlyROAM::Init()
{
	m_variance = new uint8_t[m_used_nodes];
	ComputeVariances();

	m_hypo_len = new float[m_levels];
	// todo: 0, 1 ?
	float sqrt2 = std::sqrt(2);
	float diagonal_len = m_size * sqrt2;
	m_hypo_len[2] = diagonal_len;
	for (int i = 3; i < m_levels; ++i) {
		m_hypo_len[i] = m_hypo_len[i - 1] / sqrt2;
	}
}

bool SplitOnlyROAM::Update()
{
//	AdjustQualityConstant();

	Reset();

	SplitIfNeeded(2, m_nw_tri, Pos(0, 0), Pos(m_size - 1, m_size - 1), Pos(0, m_size - 1), false, 2);
	SplitIfNeeded(3, m_se_tri, Pos(m_size - 1, m_size - 1), Pos(0, 0), Pos(m_size - 1, 0), false, 2);

	return true;
}

void SplitOnlyROAM::Draw() const
{
	if (m_nw_tri) {
		RenderTri(m_nw_tri, Pos(0, 0), Pos(m_size - 1, m_size - 1), Pos(0, m_size - 1));
		RenderTri(m_se_tri, Pos(m_size - 1, m_size - 1), Pos(0, 0), Pos(m_size - 1, 0));
	}
}

void SplitOnlyROAM::ComputeVariances()
{
	// Using 1-based numbering for levels of the bintree, level 1 is
	// the parent quad, and level 2 is each of the 2 main triangles.

	// Using 1-based numbering for the nodes, the parent quad is 1.
	// To find any nodes left and right children, use:
	//   left child: (1 << node)
	//   right child: (1 << node) + 1

	// Level1  Level2  Level3
	// +-----+ +-----+ +-----+
	// |     | |    /| |\ 5 /|
	// |     | | 2 / | | \ / |
	// |  1  | |  /  | |4 X 6|
	// |     | | / 3 | | / \ |
	// |     | |/    | |/ 7 \|
	// +-----+ +-----+ +-----+

	ComputeTriangleVariance(2, Pos(0, 0), Pos(m_size - 1, m_size - 1), Pos(0, m_size - 1), 2);
	ComputeTriangleVariance(3, Pos(m_size - 1, m_size - 1), Pos(0, 0), Pos(m_size - 1, 0), 2);
}

uint8_t SplitOnlyROAM::ComputeTriangleVariance(int num, const Pos& v0, const Pos& v1, const Pos& va, int level)
{
	uint8_t h = (m_cb.get_height(v0.x, v0.y) + m_cb.get_height(v1.x, v1.y)) >> 1;

	Pos vc((v0.x + v1.x) >> 1, (v0.y + v1.y) >> 1);
	uint8_t h_real = m_cb.get_height(vc.x, vc.y);

	uint8_t variance = std::abs(h - h_real);

	if (level < m_levels - 1)
	{
		uint8_t child_var;

		// descend to compute variance of the left child triangle
		child_var = ComputeTriangleVariance((num << 1), va, v0, vc, level + 1);
		if (child_var > variance) variance = child_var;

		// descend to compute variance of the right child triangle
		child_var = ComputeTriangleVariance((num << 1) + 1, v1, va, vc, level + 1);
		if (child_var > variance) variance = child_var;
	}

	m_variance[num] = variance;

	return variance;
}

void SplitOnlyROAM::AdjustQualityConstant()
{
	//// do we need to expand our triangle pool?
	//if (DEFAULT_POLYGON_TARGET * 3 > m_iTriPoolSize)
	//	AllocatePool();

	// The number of split triangles in the triangle pool will be roughly
	// 2x the number of drawn polygons, and provides a more robust metric
	// than using the polygon count itself.
	int diff = (m_pool.Next() / 2) - DEFAULT_POLYGON_TARGET;

	// adjust if necessary
	int range = DEFAULT_POLYGON_TARGET / 20;
	float adjust = 1.0f;
	if (diff < -range) adjust = 1.0f + ((diff + range) * ADAPTION_SPEED);
	if (diff > range) adjust = 1.0f + ((diff - range) * ADAPTION_SPEED);

	m_quality_constant *= adjust;
	if (m_quality_constant < QUALITYCONSTANT_MIN)
		m_quality_constant = QUALITYCONSTANT_MIN;
}

void SplitOnlyROAM::SplitIfNeeded(int num, BinTri *tri, const Pos& v0, const Pos& v1,
	                              const Pos& va, bool entirely_in_frustum, int level)
{
	Pos vc((v0.x + v1.x) >> 1, (v0.y + v1.y) >> 1);
	if (!entirely_in_frustum)
	{
		if (m_cb.sphere_in_frustum(vc.x, vc.y, m_hypo_len[level] / 2.0f)) {
			entirely_in_frustum = true;
		} else {
			return;
		}
	}

	// if unsplit and variance is too high, split
	if (!tri->left_child &&
		m_pool.Next() < m_pool.Size() - 50)	// safety check!  don't overflow
	{
		// do the correct split test
		float dis = m_cb.dis_to_camera(vc.x, vc.y);
		float variance = m_variance[num];
		if (variance / m_quality_constant > dis - m_hypo_len[level]) {
			Split(tri);
		}
	}

	// if now split, descend the tree
	if (tri->left_child && num < m_split_cutoff)
	{
		SplitIfNeeded((num << 1), tri->left_child, va, v0, vc, entirely_in_frustum, level+1);
		SplitIfNeeded((num << 1)+1, tri->right_child, v1, va, vc, entirely_in_frustum, level+1);
	}
}

void SplitOnlyROAM::Split(BinTri* tri)
{
	if (tri->base_neighbor != NULL)
	{
		if (tri->base_neighbor->base_neighbor != tri)
		{
			Split(tri->base_neighbor);
		}
		Split2(tri);
		Split2(tri->base_neighbor);
		tri->left_child->right_neighbor = tri->base_neighbor->right_child;
		tri->right_child->left_neighbor = tri->base_neighbor->left_child;
		tri->base_neighbor->left_child->right_neighbor = tri->right_child;
		tri->base_neighbor->right_child->left_neighbor = tri->left_child;
	}
	else
	{
		Split2(tri);
		tri->left_child->right_neighbor = NULL;
		tri->right_child->left_neighbor = NULL;
	}
}

void SplitOnlyROAM::Split2(BinTri* tri)
{
	tri->left_child = m_pool.Alloc();
	tri->right_child = m_pool.Alloc();
	tri->left_child->left_neighbor = tri->right_child;
	tri->right_child->right_neighbor = tri->left_child;
	tri->left_child->base_neighbor = tri->left_neighbor;

	// Simplified implementation by Andreas Ogren (omits 2 ifs)
	if (tri->left_neighbor != NULL)
	{
		if (tri->left_neighbor->base_neighbor == tri)
			tri->left_neighbor->base_neighbor = tri->left_child;
		else
			tri->left_neighbor->right_neighbor = tri->left_child;
	}
	tri->right_child->base_neighbor = tri->right_neighbor;
	if (tri->right_neighbor != NULL)
	{
		if (tri->right_neighbor->base_neighbor == tri)
			tri->right_neighbor->base_neighbor = tri->right_child;
		else
			tri->right_neighbor->left_neighbor = tri->right_child;
	}

	tri->left_child->left_child = NULL;
	tri->left_child->right_child = NULL;
	tri->right_child->left_child = NULL;
	tri->right_child->right_child = NULL;
}

void SplitOnlyROAM::RenderTri(BinTri* tri, const Pos& v0, const Pos& v1, const Pos& va) const
{
	if (tri->left_child)
	{
		Pos vc((v0.x + v1.x) >> 1, (v0.y + v1.y) >> 1);
		RenderTri(tri->right_child, v1, va, vc);
		RenderTri(tri->left_child, va, v0, vc);
	}
	else
	{
		m_cb.send_vertex(v0.x, v0.y);
		m_cb.send_vertex(v1.x, v1.y);
		m_cb.send_vertex(va.x, va.y);
	}
}

}