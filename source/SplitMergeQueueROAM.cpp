#include "terr/SplitMergeQueueROAM.h"

#include <cmath>

#define ADAPTION_SPEED			.00003f
#define QUALITYCONSTANT_MIN		0.002f

//frustum bitmasks
#define CULL_ALLIN 0x3f
#define CULL_OUT   0x40

//misc. diamond flags
#define ROAM_SPLIT    0x01
#define ROAM_TRI0     0x04
#define ROAM_TRI1     0x08
#define ROAM_CLIPPED  0x10
#define ROAM_SPLITQ   0x40
#define ROAM_MERGEQ   0x80
#define ROAM_ALLQ     0xc0
#define ROAM_UNQ      0x00

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

SplitMergeQueueROAM::SplitMergeQueueROAM(int size, BinTriPool& pool)
	: m_size(size)
	, m_pool(pool)
	, m_variance(nullptr)
	, m_hypo_len(nullptr)
	, m_frame_count(0)
{
	//generate a LUT for float->int conversations (huge speed-saver later on)
	float f;
	int* p_int = (int*)(&f);
	for (int i = 0; i < 256; ++i) {
		*p_int = 0x3f800000 + (i << 15);
		m_log2table[i] = (int)floor(2048 * (log(f) / log(2.0) - (float)i / 256.0) + 0.5f) << 12;
	}

	// queue
	memset(&m_split_queue[0], 0, sizeof(m_split_queue));
	memset(&m_merge_queue[0], 0, sizeof(m_merge_queue));
	m_queue_max = -1;
	m_queue_min = QUEUE_MAX_COUNT;
	m_queue_coarse = 1990; //1990 happened to be the "magic number" for the queue "fineness"

	int n = log2(m_size - 1);
	// the triangle bintree will have (2n + 2) levels
	m_levels = 2 * n + 2;
	// however, we don't need to store variance for the bottom-most nodes
	// of the tree, so use one less level, and add 1 for 1-based numbering
	m_used_nodes = (1 << (m_levels - 1));

	m_split_cutoff = (1 << (m_levels - 2));

	m_quality_constant = 0.1f;	// safe initial value

	m_drawn_tris = 0;
	m_max_tris = 30000;

	Reset();

	Enqueue(m_nw_tri, ROAM_SPLITQ, QUEUE_MAX_COUNT - 1);
	Enqueue(m_se_tri, ROAM_SPLITQ, QUEUE_MAX_COUNT - 1);
}

SplitMergeQueueROAM::~SplitMergeQueueROAM()
{
	delete[] m_variance;
	delete[] m_hypo_len;
}

void SplitMergeQueueROAM::Reset()
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

	m_nw_tri->prev = m_nw_tri->next = nullptr;
	m_se_tri->prev = m_se_tri->next = nullptr;

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
	m_nw_tri->cull_flags = 0;
	m_se_tri->cull_flags = 0;

	m_nw_tri->frame_count = (m_frame_count - 1) & 0xff;
	m_se_tri->frame_count = (m_frame_count - 1) & 0xff;

	m_nw_tri->queue_idx = QUEUE_MAX_COUNT - 1;
	m_se_tri->queue_idx = QUEUE_MAX_COUNT - 1;
}

void SplitMergeQueueROAM::AddRef(BinTri* tri)
{
	++tri->ref_count;
}

void SplitMergeQueueROAM::RemoveRef(BinTri* tri)
{
	--tri->ref_count;

	if (tri->ref_count == 0) {
		m_pool.Free(tri);
	}
}

void SplitMergeQueueROAM::Init()
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

bool SplitMergeQueueROAM::Update()
{
//	AdjustQualityConstant();

	//RecurseTesselate(m_nw_tri, false);
	//RecurseTesselate(m_se_tri, false);

	///////////////////////////////////////////////////////////////////////////////
	// update all active diamonds with a recursive culling update
//	UpdateKidsCull(m_root);

	// update all queued diamonds' priority
	static const int U = 10;	// compute 1/U of the priority updates each frame
	static int i0 = 0;
	size_t pol_sz = m_pool.GetSize();
	int i1 = i0 + (pol_sz + (U - 1)) / U;
	if (i1 >= pol_sz) {
		i1 = pol_sz - 1;
	}
	for (int i = i0; i <= i1; ++i)
	{
		auto tri = m_pool.Fetch(i);
		if (tri->flags & ROAM_ALLQ) {
			UpdatePriority(tri);
		}
	}
	i0 = (i1 + 1) % pol_sz;

	/*
	* keep splitting/merging until either
	*  (a) no split/merge priority iOverlap and:
	*      target tri count reached or accuracy target reached
	* or
	*  (b) time is up (limit optimization-loop iterations)
	* or
	*  (c) not enough free (unlocked) diamonds in cache
	*
	* Note: this implementation handles non-monotonic priorities,
	* i.e. where a child can have a higher priority than its parent.
	* Also, we are careful to be just one force-split away from being
	* beyond the target triangle/accuracy count.  As a iSide effect, this
	* eliminates one kind of oscillation that might occur if using
	* the suggested pseudocode from the original SplitMergeROAMOld paper (see Vis 1997).
	*
	*/
	const int max_opt_count = 2000; /* split/merge limit */
	int opt_count = 0;
	int overlap = m_queue_max - m_queue_min;
	int overlap0 = overlap;

#define TOO_COARSE \
	(m_drawn_tris <= m_max_tris && m_queue_max >= m_queue_coarse && m_pool.GetFreeCount() > 128)

	int side = TOO_COARSE ? -1 : 1;
	while ((side != 0 || overlap0 > 1) && opt_count < max_opt_count)
	{
		if (side <= 0)
		{
			if (m_queue_max > 0)
			{
				assert(m_queue_max >= 0 && m_queue_max < QUEUE_MAX_COUNT);
				Split(m_split_queue[m_queue_max]);
				if (!TOO_COARSE) {
					side = 1;
				}
			}
			else
			{
				side = 0;
			}
		}
		else
		{
			assert(m_queue_min >= 0 && m_queue_min < QUEUE_MAX_COUNT);
			Merge(m_merge_queue[m_queue_min]);
			if (TOO_COARSE) {
				side = 0;
			}
		}

		overlap = m_queue_max - m_queue_min;
		if (overlap < overlap0) {
			overlap0 = overlap;
		}

		opt_count++;
	}
	///////////////////////////////////////////////////////////////////////////////

	m_frame_count = (m_frame_count + 1) & 0xff;

	return true;
}

void SplitMergeQueueROAM::Draw() const
{
	m_drawn_tris = 0;
	if (m_nw_tri) {
		RenderTri(m_nw_tri, Pos(0, 0), Pos(m_size - 1, m_size - 1), Pos(0, m_size - 1));
		RenderTri(m_se_tri, Pos(m_size - 1, m_size - 1), Pos(0, 0), Pos(m_size - 1, 0));
	}
}

void SplitMergeQueueROAM::ComputeVariances()
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

uint8_t SplitMergeQueueROAM::ComputeTriangleVariance(int num, const Pos& v0, const Pos& v1, const Pos& va, int level)
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

void SplitMergeQueueROAM::AdjustQualityConstant()
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

void SplitMergeQueueROAM::RecurseTesselate(BinTri* tri, bool entirely_in_frustum)
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
		float dis = std::sqrt(m_cb.dis_to_camera_square(vc.x, vc.y));
		float variance = m_variance[tri->number];
		if (variance / m_quality_constant > dis - m_hypo_len[tri->level] && !tri->left_child) {
			Split(tri);
		}
	}
	else
	{
		float dis = std::sqrt(m_cb.dis_to_camera_square(vc.x, vc.y));
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

void SplitMergeQueueROAM::Split(BinTri* tri)
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

void SplitMergeQueueROAM::SplitNoBaseN(BinTri* tri)
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
//			assert(0);
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
//			assert(0);
		}
	}

	l->prev = l->next = nullptr;
	r->prev = r->next = nullptr;

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
	l->cull_flags = 0;
	r->cull_flags = 0;

	l->frame_count = (m_frame_count - 1) & 0xff;
	r->frame_count = (m_frame_count - 1) & 0xff;

	l->queue_idx = 0;
	r->queue_idx = 0;

	// update children queue, todo update culling
	Enqueue(l, ROAM_SPLITQ, l->queue_idx);
	UpdatePriority(l);
	Enqueue(r, ROAM_SPLITQ, r->queue_idx);
	UpdatePriority(r);

	// update parent queue
	Enqueue(tri, ROAM_MERGEQ, tri->queue_idx);
}

bool SplitMergeQueueROAM::GoodForMerge(BinTri* tri) const
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

void SplitMergeQueueROAM::Merge(BinTri* tri)
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
				// force merge base neighbor
				Merge(tri->base_neighbor);
				MergeNoBaseN(tri);
			}
		}
		return;
	}

	Merge(tri->left_child);
	Merge(tri->right_child);
}

void SplitMergeQueueROAM::MergeNoBaseN(BinTri* tri)
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

	// update children queue
	Enqueue(l, ROAM_UNQ, l->queue_idx);
	Enqueue(r, ROAM_UNQ, r->queue_idx);

	// update parent queue
	tri->flags &= ~ROAM_SPLIT;
	Enqueue(tri, ROAM_SPLITQ, tri->queue_idx);

	m_pool.Free(tri->left_child);
	m_pool.Free(tri->right_child);

	tri->left_child = nullptr;
	tri->right_child = nullptr;
}

void SplitMergeQueueROAM::UpdatePriority(BinTri* tri)
{
	// already been done
	if (m_frame_count == tri->frame_count) {
		return;
	}
	tri->frame_count = m_frame_count;

	int k;
	if (tri->flags & ROAM_CLIPPED || tri->level >= m_levels)
	{
		k = 0;
	}
	else
	{
		float d;
		int* ip = (int *)(&d); /* needed for IEEE float->int tricks */

		d = m_variance[tri->number];

		/* compute k = fixed-point log_2(r_error) (IEEE float tricks) */
		k = *ip; k += m_log2table[(k >> 15) & 0xff];

		/* compute distance from split point to camera (squared) */
		d = m_cb.dis_to_camera_square(tri->va.x, tri->va.y);

		/* compute j = fixed-point log_2(dist_to_eye) (IEEE float tricks) */
		int j = *ip; j += m_log2table[(j >> 15) & 0xff];

		/* compute k = fixed-point log_2(r_error/dist_to_eye) (more tricks) */
		k = (k - j) + 0x10000000;

		/* scale and clamp priority index to [1..IQMAX-1] */
		if (k < 0) {
			k = 0;
		}
		k = (k >> 16) + 1;
		if (k >= QUEUE_MAX_COUNT) {
			k = QUEUE_MAX_COUNT - 1;
		}

		/* for OUT diamonds, reduce the priority but leave them ordered */
		if (tri->cull_flags & CULL_OUT)
		{
			if (k > 2048)
				k -= 1024;
			else
				k = (k + 1) >> 1;
		}
	}

	Enqueue(tri, tri->flags & ROAM_ALLQ, k);
}

void SplitMergeQueueROAM::Enqueue(BinTri* tri, int queue_flags, int queue_idx)
{
	if ((tri->flags & ROAM_ALLQ) == queue_flags && tri->queue_idx == queue_idx) {
		return;
	}

	int ref_count = 0;
	if (tri->flags & ROAM_ALLQ) {
		--ref_count;
	}
	if (queue_flags & ROAM_ALLQ) {
		++ref_count;
	}

	// remove from old queue
	if (tri->flags & ROAM_ALLQ)
	{
		auto queue = (tri->flags & ROAM_SPLITQ) ? m_split_queue : m_merge_queue;
		if (tri->prev)
		{
			tri->prev->next = tri->next;
		}
		else
		{
			assert(queue[tri->queue_idx] == tri);
			queue[tri->queue_idx] = tri->next;
			// rm block, and update min max
			if (!tri->next)
			{
				if (tri->flags & ROAM_SPLITQ)
				{
					if (tri->queue_idx == m_queue_max)
					{
						auto tmp = queue[0];
						queue[0] = (BinTri*)1;
						int i;
						for (i = tri->queue_idx; !queue[i]; --i)
							;
						if (!(queue[0] = tmp) && i == 0) {
							--i;
						}
						m_queue_max = i;
					}
				}
				else
				{
					if (tri->queue_idx == m_queue_min)
					{
						auto tmp = queue[QUEUE_MAX_COUNT - 1];
						queue[QUEUE_MAX_COUNT - 1] = (BinTri*)1;
						int i;
						for (i = tri->queue_idx; !queue[i]; ++i)
							;
						if (!(queue[QUEUE_MAX_COUNT - 1] = tmp) && i == QUEUE_MAX_COUNT - 1) {
							++i;
						}
						m_queue_min = i;
					}
				}
			}
		}
		if (tri->next) {
			tri->next->prev = tri->prev;
		}
		tri->prev = tri->next = nullptr;
		tri->flags &= ~ROAM_ALLQ;
	}

	// update priority
	tri->queue_idx = queue_idx;

	// insert
	if (queue_flags & ROAM_ALLQ)
	{
		auto queue = (queue_flags & ROAM_SPLITQ) ? m_split_queue : m_merge_queue;
		tri->prev = nullptr;
		tri->next = queue[tri->queue_idx];

		queue[tri->queue_idx] = tri;
		if (tri->next)
		{
			tri->next->prev = tri;
		}
		else
		{
			if (queue_flags & ROAM_SPLITQ) {
				if (tri->queue_idx > m_queue_max) {
					m_queue_max = tri->queue_idx;
				}
			} else {
				if (tri->queue_idx < m_queue_min) {
					m_queue_min = tri->queue_idx;
				}
			}
		}

		tri->flags |= queue_flags;
	}

	if (ref_count != 0)
	{
		if (ref_count < 0) {
			RemoveRef(tri);
		} else {
			AddRef(tri);
		}
	}
}

void SplitMergeQueueROAM::RenderTri(BinTri* tri, const Pos& v0, const Pos& v1, const Pos& va) const
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
		++m_drawn_tris;
	}
}

/************************************************************************/
/* class SplitMergeQueueROAM::BinTriPool                                */
/************************************************************************/

SplitMergeQueueROAM::BinTriPool::BinTriPool(int cap)
	: m_size(cap)
	, m_next(0)
{
	m_free_count = m_size;
	m_tris = new BinTri[cap];
}

SplitMergeQueueROAM::BinTriPool::~BinTriPool()
{
	delete[] m_tris;
}

SplitMergeQueueROAM::BinTri* SplitMergeQueueROAM::BinTriPool::Alloc()
{
	--m_free_count;
	SplitMergeQueueROAM::BinTri* ret = nullptr;
	if (m_freelist)
	{
		ret = m_freelist;
		m_freelist = m_freelist->next;
		if (m_freelist) {
			m_freelist->prev = nullptr;
		}
	}
	else
	{
		assert(m_next < m_size);
		ret = &m_tris[m_next++];
	}
	assert(!ret->prev);
	ret->prev = ret->next = nullptr;
	ret->ref_count = 1;
	return ret;
}

void SplitMergeQueueROAM::BinTriPool::Free(BinTri* tri)
{
	++m_free_count;
	if (tri->prev) {
		tri->prev->next = tri->next;
	}
	if (tri->next) {
		tri->next->prev = tri->prev;
	}
	tri->prev = nullptr;
	if (m_freelist) {
		tri->next = m_freelist;
	} else {
		tri->next = nullptr;
	}
	m_freelist = tri;
}

SplitMergeQueueROAM::BinTri* SplitMergeQueueROAM::BinTriPool::Fetch(int idx)
{
	if (idx >= 0 && idx < m_size) {
		return &m_tris[idx];
	} else {
		return nullptr;
	}
}

void SplitMergeQueueROAM::BinTriPool::Reset()
{
	m_free_count = m_size;
	m_next = 0;
	m_freelist = nullptr;
}

}