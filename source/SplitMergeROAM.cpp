#include "terr/SplitMergeROAM.h"

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

SplitMergeROAM::SplitMergeROAM(int size, BinTriPool& pool)
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

	Reset();
}

SplitMergeROAM::~SplitMergeROAM()
{
	delete[] m_variance;
	delete[] m_hypo_len;
}

void SplitMergeROAM::Reset()
{
	m_pool.Reset();

	m_nw_tri = m_pool.Alloc();
	m_se_tri = m_pool.Alloc();

	m_nw_tri->left_child     = nullptr;
	m_nw_tri->right_child    = nullptr;
	m_nw_tri->left_neighbor  = nullptr;
	m_nw_tri->right_neighbor = nullptr;
	m_nw_tri->base_neighbor  = m_se_tri;
	m_nw_tri->parent         = nullptr;

	m_se_tri->left_child     = nullptr;
	m_se_tri->right_child    = nullptr;
	m_se_tri->left_neighbor  = nullptr;
	m_se_tri->right_neighbor = nullptr;
	m_se_tri->base_neighbor  = m_nw_tri;
	m_se_tri->parent         = nullptr;

	m_nw_tri->next = nullptr;
	m_se_tri->next = nullptr;

	m_nw_tri->v0.Assign(0, 0);
	m_nw_tri->v1.Assign(m_size - 1, m_size - 1);
	m_nw_tri->va.Assign(0, m_size - 1);
	m_nw_tri->level = 2;
	m_nw_tri->number = 2;

	m_se_tri->v0.Assign(m_size - 1, m_size - 1);
	m_se_tri->v1.Assign(0, 0);
	m_se_tri->va.Assign(m_size - 1, 0);
	m_se_tri->level = 2;
	m_se_tri->number = 3;

	m_nw_tri->flags = 0;
	m_se_tri->flags = 0;
}

void SplitMergeROAM::Init()
{
	m_variance = new uint8_t[m_used_nodes];
	ComputeVariances();

	m_hypo_len = new float[m_levels];
	// todo: 0, 1 ?
	float sqrt2 = static_cast<float>(std::sqrt(2));
	float diagonal_len = m_size * sqrt2;
	m_hypo_len[2] = diagonal_len;
	for (int i = 3; i < m_levels; ++i) {
		m_hypo_len[i] = m_hypo_len[i - 1] / sqrt2;
	}
}

bool SplitMergeROAM::Update()
{
//	AdjustQualityConstant();

	RecurseTesselate(m_nw_tri, false);
	RecurseTesselate(m_se_tri, false);

	return true;
}

void SplitMergeROAM::Draw() const
{
	if (m_nw_tri) {
		RenderTri(m_nw_tri, Pos(0, 0), Pos(m_size - 1, m_size - 1), Pos(0, m_size - 1));
		RenderTri(m_se_tri, Pos(m_size - 1, m_size - 1), Pos(0, 0), Pos(m_size - 1, 0));
	}
}

void SplitMergeROAM::ComputeVariances()
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

uint8_t SplitMergeROAM::ComputeTriangleVariance(int num, const Pos& v0, const Pos& v1, const Pos& va, int level)
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

void SplitMergeROAM::AdjustQualityConstant()
{
	//// do we need to expand our triangle pool?
	//if (DEFAULT_POLYGON_TARGET * 3 > m_iTriPoolSize)
	//	AllocatePool();

	// The number of split triangles in the triangle pool will be roughly
	// 2x the number of drawn polygons, and provides a more robust metric
	// than using the polygon count itself.
	int diff = (m_pool.GetNext() / 2) - DEFAULT_POLYGON_TARGET;

	// adjust if necessary
	int range = DEFAULT_POLYGON_TARGET / 20;
	float adjust = 1.0f;
	if (diff < -range) adjust = 1.0f + ((diff + range) * ADAPTION_SPEED);
	if (diff > range) adjust = 1.0f + ((diff - range) * ADAPTION_SPEED);

	m_quality_constant *= adjust;
	if (m_quality_constant < QUALITYCONSTANT_MIN)
		m_quality_constant = QUALITYCONSTANT_MIN;
}

void SplitMergeROAM::RecurseTesselate(BinTri* tri, bool entirely_in_frustum)
{
	Pos vc((tri->v0.x + tri->v1.x) >> 1, (tri->v0.y + tri->v1.y) >> 1);
	if (!entirely_in_frustum)
	{
		if (m_cb.sphere_in_frustum(vc.x, vc.y, m_hypo_len[tri->level] / 2.0f)) {
			entirely_in_frustum = true;
		} else {
			return;
		}
	}

	// if unsplit and variance is too high, split
	if (!tri->left_child &&
		m_pool.GetNext() < m_pool.GetSize() - 50)	// safety check!  don't overflow
	{
		// do the correct split test
		float dis = m_cb.dis_to_camera(vc.x, vc.y);
		float variance = m_variance[tri->number];
		if (variance / m_quality_constant > dis - m_hypo_len[tri->level] && !tri->left_child) {
			Split(tri);
		}
	}
	else
	{
		float dis = m_cb.dis_to_camera(vc.x, vc.y);
		float variance = m_variance[tri->number];
		if (variance / m_quality_constant < dis - m_hypo_len[tri->level] && tri->left_child) {
			Merge(tri);
		}
	}

	// if now split, descend the tree
	if (tri->left_child && tri->number < m_split_cutoff)
	{
		RecurseTesselate(tri->left_child, entirely_in_frustum);
		RecurseTesselate(tri->right_child, entirely_in_frustum);
	}
}

void SplitMergeROAM::Split(BinTri* tri)
{
	// already split
	if (tri->left_child) {
		return;
	}

	if (tri->base_neighbor != nullptr)
	{
		if (tri->base_neighbor->base_neighbor != tri) {
			Split(tri->base_neighbor);
		}
		SplitNoBaseN(tri);
		SplitNoBaseN(tri->base_neighbor);
		tri->left_child->right_neighbor = tri->base_neighbor->right_child;
		tri->right_child->left_neighbor = tri->base_neighbor->left_child;
		tri->base_neighbor->left_child->right_neighbor = tri->right_child;
		tri->base_neighbor->right_child->left_neighbor = tri->left_child;
	}
	else
	{
		SplitNoBaseN(tri);
	}
}

void SplitMergeROAM::SplitNoBaseN(BinTri* tri)
{
	if (tri->left_child) {
		return;
	}

	auto l = m_pool.Alloc();
	auto r = m_pool.Alloc();
	tri->left_child  = l;
	tri->right_child = r;

	l->parent = tri;
	r->parent = tri;

	l->left_neighbor  = r;
	r->right_neighbor = l;
	l->right_neighbor = nullptr;
	r->left_neighbor  = nullptr;

	l->left_child  = nullptr;
	l->right_child = nullptr;
	r->left_child  = nullptr;
	r->right_child = nullptr;

	// connect with neighbor
	l->base_neighbor  = tri->left_neighbor;
	r->base_neighbor = tri->right_neighbor;
	if (tri->left_neighbor != nullptr)
	{
		if (tri->left_neighbor->base_neighbor == tri) {
			tri->left_neighbor->base_neighbor = l;
		} else if (tri->left_neighbor->left_neighbor == tri) {
			tri->left_neighbor->left_neighbor = l;
		} else if (tri->left_neighbor->right_neighbor == tri) {
			tri->left_neighbor->right_neighbor = l;
		} else {
			assert(0);
		}
	}
	if (tri->right_neighbor != nullptr)
	{
		if (tri->right_neighbor->base_neighbor == tri) {
			tri->right_neighbor->base_neighbor = r;
		} else if (tri->right_neighbor->left_neighbor == tri) {
			tri->right_neighbor->left_neighbor = r;
		} else if (tri->right_neighbor->right_neighbor == tri) {
			tri->right_neighbor->right_neighbor = r;
		} else {
			assert(0);
		}
	}

	l->next = nullptr;
	r->next = nullptr;

	Pos vc((tri->v0.x + tri->v1.x) >> 1, (tri->v0.y + tri->v1.y) >> 1);

	l->v0 = tri->va;
	l->v1 = tri->v0;
	l->va = vc;
	l->level = tri->level + 1;
	l->number = (tri->number << 1);

	r->v0 = tri->v1;
	r->v1 = tri->va;
	r->va = vc;
	r->level = tri->level + 1;
	r->number = (tri->number << 1) + 1;

	l->flags = 0;
	r->flags = 0;
}

bool SplitMergeROAM::GoodForMerge(BinTri* tri) const
{
	// already merged
	if (tri->left_child == nullptr) {
		return false;
	}

	// todo variance

	if (tri->left_child->left_child == nullptr &&
		tri->right_child->left_child == nullptr) {
		return true;
	}

	return false;
}

void SplitMergeROAM::Merge(BinTri* tri)
{
	// already merged
	if (tri->left_child == nullptr) {
		return;
	}

	if (GoodForMerge(tri))
	{
		if (tri->base_neighbor == nullptr)
		{
			MergeNoBaseN(tri);
		}
		else
		{
			if (GoodForMerge(tri->base_neighbor))
			{
				MergeNoBaseN(tri->base_neighbor);
				MergeNoBaseN(tri);
			}
			else
			{
				//// force merge base neighbor
				//Merge(tri->base_neighbor);
				//MergeNoBaseN(tri);
			}
		}
		return;
	}

	Merge(tri->left_child);
	Merge(tri->right_child);
}

void SplitMergeROAM::MergeNoBaseN(BinTri* tri)
{
	auto l = tri->left_child;
	if (l->base_neighbor)
	{
		if (l->base_neighbor->left_neighbor == tri->left_child) {
			l->base_neighbor->left_neighbor = tri;
		}
		if (l->base_neighbor->right_neighbor == tri->left_child) {
			l->base_neighbor->right_neighbor = tri;
		}
		if (l->base_neighbor->base_neighbor == tri->left_child) {
			l->base_neighbor->base_neighbor = tri;
			if (tri->left_neighbor == l->base_neighbor->parent) {
				tri->left_neighbor = l->base_neighbor;
			}
		}

		auto p = l->base_neighbor->parent;
		if (p)
		{
			if (p->left_neighbor == tri->left_child) {
				p->left_neighbor = tri;
			}
			if (p->right_neighbor == tri->left_child) {
				p->right_neighbor = tri;
			}
			if (p->base_neighbor == tri->left_child) {
				p->base_neighbor = tri;
			}
		}
	}

	auto r = tri->right_child;
	if (r->base_neighbor)
	{
		if (r->base_neighbor->left_neighbor == tri->right_child) {
			r->base_neighbor->left_neighbor = tri;
		}
		if (r->base_neighbor->right_neighbor == tri->right_child) {
			r->base_neighbor->right_neighbor = tri;
		}
		if (r->base_neighbor->base_neighbor == tri->right_child) {
			r->base_neighbor->base_neighbor = tri;
			if (tri->right_neighbor == r->base_neighbor->parent) {
				tri->right_neighbor = r->base_neighbor;
			}
		}

		auto p = r->base_neighbor->parent;
		if (p)
		{
			if (p->left_neighbor == tri->right_child) {
				p->left_neighbor = tri;
			}
			if (p->right_neighbor == tri->right_child) {
				p->right_neighbor = tri;
			}
			if (p->base_neighbor == tri->right_child) {
				p->base_neighbor = tri;
			}
		}
	}

	m_pool.Free(tri->left_child);
	m_pool.Free(tri->right_child);
	tri->left_child = nullptr;
	tri->right_child = nullptr;
}

void SplitMergeROAM::RenderTri(BinTri* tri, const Pos& v0, const Pos& v1, const Pos& va) const
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

/************************************************************************/
/* class SplitMergeROAM::BinTriPool                                     */
/************************************************************************/

SplitMergeROAM::BinTriPool::BinTriPool(int cap)
	: m_size(cap)
	, m_next(0)
	, m_freelist(nullptr)
{
	m_tris = new BinTri[cap];
}

SplitMergeROAM::BinTriPool::~BinTriPool()
{
	delete[] m_tris;
}

SplitMergeROAM::BinTri* SplitMergeROAM::BinTriPool::Alloc()
{
	if (m_freelist)
	{
		auto ret = m_freelist;
		m_freelist = m_freelist->next;
		return ret;
	}
	else
	{
		assert(m_next < m_size);
		return &m_tris[m_next++];
	}
}

void SplitMergeROAM::BinTriPool::Free(BinTri* tri)
{
	if (m_freelist) {
		tri->next = m_freelist;
	} else {
		tri->next = nullptr;
	}
	m_freelist = tri;
}

void SplitMergeROAM::BinTriPool::Reset()
{
	m_next = 0;
	m_freelist = nullptr;
}

}