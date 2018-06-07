// code from http://www.cognigraph.com/ROAM_homepage/
//
//#include "terr/SplitMergeROAM.h"
//
//#include <cmath>
//#include <iostream>
//
//#include <string.h>
//
//namespace terr
//{
//
//SplitMergeROAM::SplitMergeROAM(int max_level, int pool_size, int size,
//	                           const pt3::Camera& cam, const pt3::ViewFrustum& vf,
//	                           std::function<float(float, float)> get_height_cb)
//	: m_camera(cam)
//	, m_frustum(vf)
//	, m_get_height_cb(get_height_cb)
//{
//	m_size = size;
//
//	m_pool_size = pool_size;
//	m_pool = new Diamond[m_pool_size];
//	// init diamond
//	for (int i = 0; i < m_pool_size; ++i)
//	{
//		auto& dm = m_pool[i];
//		memset(&dm, 0, sizeof(dm));
//
//		dm.queue_idx = QUEUE_CAP / 2;
//		dm.bound_rad = -1;
//		dm.error_rad = 10;
//		dm.frame_count = 255;
//		dm.level = -100;
//	}
//	// connect
//	Diamond *temp_dm0, *temp_dm1;
//	for (int i = 0; i < m_pool_size - 1; ++i)
//	{
//		temp_dm0 = m_pool + i;
//		temp_dm1 = m_pool + i + 1;
//
//		temp_dm0->next = temp_dm1;
//		temp_dm1->prev = temp_dm0;
//	}
//	m_pool_begin = m_pool;
//	m_pool_end = m_pool + m_pool_size;
//	m_pool_begin->prev = nullptr;
//	m_pool_end->next = nullptr;
//
//	m_free_count = m_pool_size;
//
//	m_max_level = max_level;
//	m_level_md_size = new float[m_max_level + 1];
//	for (int i = 0; i <= m_max_level; ++i) {
//		m_level_md_size[i] = 255.0f / ((float)std::sqrt((float)((__int64)1 << i)));
//	}
//
//	m_max_tri_chunks = TRI_IMAX;
//	m_tri_dm_ij = new int[m_max_tri_chunks];
//	m_vert_buf = new float[m_max_tri_chunks * 15];
//	for (int i = 0; i < m_max_tri_chunks; ++i) {
//		m_tri_dm_ij[i] = -1;
//	}
//	m_free_tri = 1;
//	m_free_tri_count = m_max_tri_chunks - 1;
//	m_max_tris = 30000;			//set the default maximum triangle count (max triangles visible per frame)
//
//	memset(&m_split_queue[0], 0, sizeof(m_split_queue));
//	memset(&m_merge_queue[0], 0, sizeof(m_merge_queue));
//	m_queue_max = -1;
//	m_queue_min = QUEUE_CAP;
//	m_queue_coarse = 1990; //1990 happened to be the "magic number" for the queue "fineness"
//
//	//generate a LUT for float->int conversations (huge speed-saver later on)
//	float f;
//	int* pint = (int*)(&f);
//	for (int i = 0; i < 256; ++i)
//	{
//		*pint = 0x3f800000 + (i << 15);
//		m_log2table[i] = (int)floor(m_size*(log(f) / log(2.0) - (float)i / 256.0) + 0.5f) << 12;
//	}
//
//	m_root = NewDiamond();
//
//	m_verts_per_frame = 0;
//	m_tris_per_frame = 0;
//}
//
//SplitMergeROAM::~SplitMergeROAM()
//{
//	delete[] m_pool;
//	delete[] m_level_md_size;
//	delete[] m_vert_buf;
//	delete[] m_tri_dm_ij;
//}
//
//void SplitMergeROAM::Update()
//{
//	// update all active diamonds with a recursive culling update
//	UpdateKidsCull(m_root);
//
//	// update all queued diamonds' priority
//	static const int U = 10;	// compute 1/U of the priority updates each frame
//	static int i0 = 0;
//	int i1 = i0 + (m_pool_size + (U - 1)) / U;
//	if (i1 >= m_pool_size) {
//		i1 = m_pool_size - 1;
//	}
//	for (int i = i0; i <= i1; ++i)
//	{
//		Diamond* dm = m_pool + i;
//		if (dm->flags & ROAM_ALLQ) {
//			UpdatePriority(dm);
//		}
//	}
//	i0 = (i1 + 1) % m_pool_size;
//
//	/*
//	* keep splitting/merging until either
//	*  (a) no split/merge priority iOverlap and:
//	*      target tri count reached or accuracy target reached
//	* or
//	*  (b) time is up (limit optimization-loop iterations)
//	* or
//	*  (c) not enough free (unlocked) diamonds in cache
//	*
//	* Note: this implementation handles non-monotonic priorities,
//	* i.e. where a child can have a higher priority than its parent.
//	* Also, we are careful to be just one force-split away from being
//	* beyond the target triangle/accuracy count.  As a iSide effect, this
//	* eliminates one kind of oscillation that might occur if using
//	* the suggested pseudocode from the original SplitMergeROAM paper (see Vis 1997).
//	*
//	*/
//	const int max_opt_count = 2000; /* split/merge limit */
//	int opt_count = 0;
//	int overlap = m_queue_max - m_queue_min;
//	int overlap0 = overlap;
//
//#define TOO_COARSE \
//	(m_tris_per_frame <= m_max_tris && m_queue_max >= m_queue_coarse && \
//	 m_free_count > 128 && m_free_tri_count > 128)
//
//	int side = TOO_COARSE ? -1 : 1;
//	while ((side != 0 || overlap0 > 1) && opt_count < max_opt_count)
//	{
//		if (side <= 0)
//		{
//			if (m_queue_max > 0)
//			{
//				Split(m_split_queue[m_queue_max]);
//				if (!TOO_COARSE) {
//					side = 1;
//				}
//			}
//			else
//			{
//				side = 0;
//			}
//		}
//		else
//		{
//			Merge(m_merge_queue[m_queue_min]);
//			if (TOO_COARSE) {
//				side = 0;
//			}
//		}
//
//		overlap = m_queue_max - m_queue_min;
//		if (overlap < overlap0) {
//			overlap0 = overlap;
//		}
//
//		opt_count++;
//	}
//
//	m_frame_count = (m_frame_count + 1) & 255;
//}
//
//SplitMergeROAM::Diamond* SplitMergeROAM::NewDiamond()
//{
//	Diamond* dm = m_pool_begin;
//	if (!dm) {
//		std::cerr << "No space: diamond pool." << std::endl;
//		exit(1);
//	}
//
//	/* if not NEW, unlink from parents, otherwise set to not NEW */
//	if (dm->bound_rad >= 0.0f)
//	{
//		dm->p[0]->k[dm->i[0]] = nullptr; RemoveRef(dm->p[0]);
//		dm->p[1]->k[dm->i[1]] = nullptr; RemoveRef(dm->p[1]);
//		dm->queue_idx = QUEUE_CAP >> 1;
//	}
//	else
//	{
//		dm->bound_rad = 0.0f;
//	}
//
//	// make sure the framecnt is old
//	dm->frame_count = (m_free_count - 1) & 255;
//
//	AddRef(dm);
//
//	return dm;
//}
//
//void SplitMergeROAM::AddRef(Diamond* dm)
//{
//	if (dm->ref_count == 0)
//	{
//		Diamond* prev = dm->prev;
//		Diamond* next = dm->next;
//
//		if (prev) {
//			prev->next = dm->next;
//		} else {
//			m_pool_begin = next;
//		}
//
//		if (next) {
//			next->prev = dm->prev;
//		} else {
//			m_pool_end = prev;
//		}
//
//		--m_free_count;
//	}
//
//	++dm->ref_count;
//}
//
//void SplitMergeROAM::RemoveRef(Diamond* dm)
//{
//	--dm->ref_count;
//
//	if (dm->ref_count == 0)
//	{
//		Diamond* prev = m_pool_end;
//
//		dm->prev = prev;
//		dm->next = nullptr;
//
//		if (prev) {
//			prev->next = dm;
//		} else {
//			m_pool_begin = dm;
//		}
//
//		m_pool_end = dm;
//
//		++m_free_count;
//	}
//}
//
//void SplitMergeROAM::AllocateTri(Diamond* dm, int j)
//{
//	//CLIPPED diamonds never have triangles
//	if (dm->flags & ROAM_CLIPPED) {
//		return;
//	}
//
//	int flags = dm->p[j]->flags;
//
//	//CLIPPED parent j means no triangle on iSide j
//	if (flags & ROAM_CLIPPED) {
//		return;
//	}
//
//	//indicate that the triangle on side j is active
//	dm->flags |= ROAM_TRI0 << j;
//
//	//if not IN, take the triangle off of the OUT list
//	if (!(dm->cull & CULL_OUT)) {
//		AddTri(dm, j);
//	}
//}
//
//void SplitMergeROAM::FreeTri(Diamond* dm, int j)
//{
//	//CLIPPED diamonds never have triangles
//	if (dm->flags & ROAM_CLIPPED) {
//		return;
//	}
//
//	int flags = dm->p[j]->flags;
//
//	//CLIPPED parent j means no triangle on iSide j
//	if (flags & ROAM_CLIPPED) {
//		return;
//	}
//
//	//indicate that the triangle on iSide j is not active
//	dm->flags &= ~(ROAM_TRI0 << j);
//
//	//if not OUT, take the triangle off of the IN list
//	if (!(dm->cull & CULL_OUT)) {
//		RemoveTri(dm, j);
//	}
//}
//
//void SplitMergeROAM::AddTri(Diamond* dm, int j)
//{
//	/* grab free tri and fill in */
//	//get a free triangle and "fill" it in
//	int i = m_free_tri++;
//	if (i >= m_max_tri_chunks)
//	{
//		std::cerr << "roam: out of triangle memory\n";
//		/* should deal with this more gracefully... */
//		exit(1);
//	}
//	m_free_tri_count--;
//	dm->tri_idx[j] = i;
//	m_tri_dm_ij[i] = ((dm - m_pool) << 1) | j;
//
//	//fill in the information for the triangle
//	Diamond* dmnd_table[3];
//	dmnd_table[1] = dm->p[j];
//	if (j)
//	{
//		dmnd_table[0] = dm->a[1];
//		dmnd_table[2] = dm->a[0];
//	}
//	else
//	{
//		dmnd_table[0] = dm->a[0];
//		dmnd_table[2] = dm->a[1];
//	}
//
//	//fill the vertex buffer with the information
//	float* vb = (float*)m_vert_buf + 15 * i;
//	for (int vi = 0; vi < 3; vi++, vb += 5)
//	{
//		vb[2] = dmnd_table[vi]->pos[0];
//		vb[3] = dmnd_table[vi]->pos[1];
//		vb[4] = dmnd_table[vi]->pos[2];
//
//		vb[0] = vb[2] / m_size;
//		vb[1] = vb[4] / m_size;
//	}
//
//	m_verts_per_frame += 3;
//	m_tris_per_frame++;
//}
//
//void SplitMergeROAM::RemoveTri(Diamond* dm, int j)
//{
//	int i = dm->tri_idx[j];
//
//	//put the triangle back on the "free" list for use
//	dm->tri_idx[j] = 0;
//	m_free_tri--;
//	m_free_count++;
//
//	//copy the last triangle on the list (non-free) to the freed triangle
//	int ix = m_free_tri;
//	int dmnd_ij = m_tri_dm_ij[ix];
//	int jx = dmnd_ij & 1;
//	Diamond* dmnd_x = m_pool + (dmnd_ij >> 1);
//
//	dmnd_x->tri_idx[jx] = i;
//	m_tri_dm_ij[i] = dmnd_ij;
//
//	float* vb = (float*)m_vert_buf;
//	float* vb_idx = vb + 15 * ix;
//	vb += 15 * i;
//	memcpy((void*)vb, (void*)vb_idx, 15 * sizeof(float));
//
//	m_verts_per_frame -= 3;
//	m_tris_per_frame--;
//}
//
//SplitMergeROAM::Diamond* SplitMergeROAM::GetKid(Diamond* c, int i)
//{
//	if (auto k = c->k[i]) {
//		AddRef(k);
//		return k;
//	}
//
//	// avoid early recycling
//	AddRef(c);
//
//	// recursively create other parent to kid i
//	// c's parent on kid side
//	Diamond* px;
//	int ix;
//	if (i < 2)
//	{
//		px = c->p[0];
//		ix = (c->i[0] + (i == 0 ? 1 : -1)) & 3;
//	}
//	else
//	{
//		px = c->p[1];
//		ix = (c->i[1] + (i == 2 ? 1 : -1)) & 3;
//	}
//	// c's brother in parent
//	Diamond* cx = GetKid(px, ix);
//
//	// return
//	Diamond* k = NewDiamond();
//
//	// set all the links
//	c->k[i] = k;
//	ix = (i & 1) ^ 1;
//	if (cx->p[1] == px) {
//		ix |= 2;
//	} else {
//		cx->k[ix] = k;
//	}
//	if (i & 1)
//	{
//		k->p[0] = cx;
//		k->i[0] = ix;
//		k->p[1] = c;
//		k->i[1] = i;
//	}
//	else
//	{
//		k->p[0] = c;
//		k->i[0] = i;
//		k->p[1] = cx;
//		k->i[1] = ix;
//	}
//	k->a[0] = c->p[i >> 1];
//	k->a[1] = c->a[((i + 1) & 2) >> 1];
//	k->k[0] = k->k[1] = k->k[2] = k->k[3] = nullptr;
//
//	// flags
//	k->cull = 0;
//	k->flags = 0;
//	k->split_flags = 0;
//	// todo ?
//	if ((k->a[0]->flags & ROAM_CLIPPED) ||
//		((c->flags & ROAM_CLIPPED) && (cx->flags & ROAM_CLIPPED))) {
//		k->flags |= ROAM_CLIPPED;
//	}
//	k->queue_idx = -10;
//	k->level = c->level + 1;
//
//	// vertex
//	float* a0_pos = k->a[0]->pos;
//	float* a1_pos = k->a[1]->pos;
//	k->pos[0] = (a0_pos[0] + a1_pos[0]) * 0.5f;
//	k->pos[2] = (a0_pos[2] + a1_pos[2]) * 0.5f;
//	k->pos[1] = m_get_height_cb(k->pos[0], k->pos[1]);
////	float* center = k->pos;
//
//	// rad
//	float* vc = k->pos;
//	float rd, rc, dx, dy, dz;
//	float* v;
//	v = k->p[0]->pos; dx = v[0] - vc[0]; dy = v[1] - vc[1]; dz = v[2] - vc[2];
//	rd = dx * dx + dy * dy + dz * dz;
//	v = k->p[1]->pos; dx = v[0] - vc[0]; dy = v[1] - vc[1]; dz = v[2] - vc[2];
//	rc = dx * dx + dy * dy + dz * dz; if (rc>rd) rd = rc;
//	v = k->a[0]->pos; dx = v[0] - vc[0]; dy = v[1] - vc[1]; dz = v[2] - vc[2];
//	rc = dx * dx + dy * dy + dz * dz; if (rc>rd) rd = rc;
//	v = k->a[1]->pos; dx = v[0] - vc[0]; dy = v[1] - vc[1]; dz = v[2] - vc[2];
//	rc = dx * dx + dy * dy + dz * dz; if (rc>rd) rd = rc;
//	k->bound_rad = rd;
//	k->error_rad = std::pow(m_level_md_size[k->level], 2);
//
//	return k;
//}
//
//void SplitMergeROAM::Enqueue(Diamond* dm, char queue_flag, int new_idx)
//{
//	if (dm->flags & ROAM_ALLQ && dm->queue_idx == new_idx) {
//		return;
//	}
//
//	int ref_count = 0;
//	if (dm->flags & ROAM_ALLQ) {
//		--ref_count;
//	}
//	if (queue_flag & ROAM_ALLQ) {
//		++ref_count;
//	}
//
//	// remove from old queue
//	if (dm->flags & ROAM_ALLQ)
//	{
//		auto& queue = (queue_flag & ROAM_SPLITQ) ? m_split_queue : m_merge_queue;
//		if (dm->prev) {
//			dm->prev->next = dm->next;
//		} else {
//			queue[dm->queue_idx] = dm->next;
//			if (!dm->next)
//			{
//				if (dm->flags & ROAM_SPLITQ)
//				{
//					if (dm->queue_idx == m_queue_max)
//					{
//						auto tmp = queue[0];
//						queue[0] = (Diamond*)1;
//						int i;
//						for (i = dm->queue_idx; !queue[i]; --i)
//							;
//						if (!(queue[0] = tmp) && i == 0) {
//							--i;
//						}
//						m_queue_max = i;
//					}
//				}
//				else
//				{
//					if (dm->queue_idx == m_queue_min)
//					{
//						auto tmp = queue[QUEUE_CAP - 1];
//						queue[QUEUE_CAP - 1] = (Diamond*)1;
//						int i;
//						for (i = dm->queue_idx; !queue[i]; ++i)
//							;
//						if (!(queue[QUEUE_CAP - 1] = tmp) && i == QUEUE_CAP - 1) {
//							++i;
//						}
//						m_queue_min = i;
//					}
//				}
//			}
//		}
//	}
//
//	// update priority
//	dm->queue_idx = new_idx;
//
//	// insert
//	if (queue_flag & ROAM_ALLQ)
//	{
//		auto& queue = (queue_flag & ROAM_SPLITQ) ? m_split_queue : m_merge_queue;
//		dm->prev = nullptr;
//		dm->next = queue[dm->queue_idx];
//
//		queue[dm->queue_idx] = dm;
//		if (dm->next) {
//			dm->next->prev = dm;
//		} else {
//			if (queue_flag & ROAM_SPLITQ) {
//				if (dm->queue_idx > m_queue_max) {
//					m_queue_max = dm->queue_idx;
//				}
//			} else {
//				if (dm->queue_idx < m_queue_min) {
//					m_queue_min = dm->queue_idx;
//				}
//			}
//		}
//
//		dm->flags |= queue_flag;
//	}
//
//	if (ref_count != 0)
//	{
//		if (ref_count < 0) {
//			RemoveRef(dm);
//		} else {
//			AddRef(dm);
//		}
//	}
//}
//
//void SplitMergeROAM::Split(Diamond* dm)
//{
////	SROAM_DIAMOND* pChild, *pParent;
////	int i, s;
//
//	//if the diamond has already been split, then skip it! And skip it good!
//	if (dm->flags & ROAM_SPLIT) {
//		return;
//	}
//
//	//split parents recursively (as needed)
//	for(int i = 0; i < 2; i++)
//	{
//		Diamond* p = dm->p[i];
//		Split(p);
//
//		//if the diamond is p's first split child, take p off of the merge queue
//		if (!(p->split_flags & SPLIT_K)) {
//			Enqueue(p, ROAM_UNQ, p->queue_idx);
//		}
//		p->split_flags |= SPLIT_K0 << dm->i[i];
//	}
//
//	//get the children, update flags, and put on the split queue
//	for(int i = 0; i < 4; i++)
//	{
//		Diamond* k = GetKid(dm, i);
//		UpdateCull(k);
//		UpdatePriority(k);
//
//		//children of the newly split diamond now go on the split queue
//		Enqueue(k, ROAM_SPLITQ, k->queue_idx);
//		int s= k->p[1] == dm ? 1 :0 ;
//		k->split_flags |= SPLIT_P0 << s;
//		RemoveRef(k);
//
//		//put the child triangles on the render list
//		AllocateTri(k, s);
//	}
//
//	//diamond is split, update it's queue, and add to "check list"
//	dm->flags |= ROAM_SPLIT;
//	Enqueue(dm, ROAM_MERGEQ, dm->queue_idx);	//newly split diamond goes on merge queue
//
//	//put parent tris back on the free list
//	FreeTri(dm, 0);
//	FreeTri(dm, 1);
//}
//
//void SplitMergeROAM::Merge(Diamond* dm)
//{
//	//	SROAM_DIAMOND* k, *pParent;
//	//int i, s;
//
//	//if this diamond has already been merged, then skip
//	if (!(dm->flags & ROAM_SPLIT)) {
//		return;
//	}
//
//	//children off split queue if their other parent is not split
//	for(int i = 0; i < 4; i++)
//	{
//		Diamond* k = dm->k[i];
//		int s = k->p[1] == dm ? 1 : 0;
//
//		k->split_flags &= ~(SPLIT_P0 << s);
//		if (!(k->split_flags & SPLIT_P)) {
//			Enqueue(k, ROAM_UNQ, k->queue_idx);
//		}
//
//		//put the tris back on the free list
//		FreeTri( k, s );
//	}
//
//	//diamond is not split, update it's queue, and add to "check list"
//	dm->flags &= ~ROAM_SPLIT;
//	Enqueue(dm, ROAM_SPLITQ, dm->queue_idx);
//
//	//update the diamond's parents, only if it is needed
//	for (int i = 0; i < 2; i++)
//	{
//		Diamond* p = dm->p[i];
//
//		p->split_flags &= ~(SPLIT_K0 << dm->i[i]);
//		if (!(p->split_flags & SPLIT_K)) {
//			UpdatePriority(p);
//			Enqueue(p, ROAM_MERGEQ, p->queue_idx);
//		}
//	}
//
//	//put the parent tris on the triangle render list
//	AllocateTri(dm, 0);
//	AllocateTri(dm, 1);
//}
//
//void SplitMergeROAM::UpdateKidsCull(Diamond* dm)
//{
//	// CLIPPED diamonds have no interest here, back out
//	if (dm->flags & ROAM_CLIPPED) {
//		return;
//	}
//
//	int cull = dm->cull; //save old culling flag for comparison
//
//	// update the diamond's culling flags
//	UpdateCull(dm);
//
//	// skip subtree if nothing has really changed
//	if (cull == dm->cull && (cull == CULL_OUT || cull == CULL_ALLIN)) {
//		return;
//	}
//
//	//update diamond priority if culling OUT state has changed
//	if ((cull ^ dm->cull) & CULL_OUT) {
//		UpdatePriority(dm);
//	}
//
//	//if diamond is split, recurse down to it's four children if they exist
//	if(dm->flags & ROAM_SPLIT)
//	{
//		for(int i = 0; i < 4; i+=2)
//		{
//			if (auto k = dm->k[i])
//			{
//				if (k->p[0] == dm)
//				{
//					if (k->k[0]) {
//						UpdateKidsCull(k->k[0]);
//					}
//					if (k->k[1]) {
//						UpdateKidsCull(k->k[1]);
//					}
//				}
//				else
//				{
//					if (k->k[2]) {
//						UpdateKidsCull(k->k[2]);
//					}
//					if (k->k[3]) {
//						UpdateKidsCull(k->k[3]);
//					}
//				}
//			}
//		}
//	}
//}
//
//void SplitMergeROAM::UpdateCull(Diamond* dm)
//{
//	//get the diamond's parent's culling flag
//	int cull = dm->a[0]->cull;
//
//	//if needed, update for all non-IN halfspaces
//	if (cull != CULL_ALLIN && cull != CULL_OUT)
//	{
//		auto& frustum = m_frustum.GetViewFrustum();
//		for (int j = 0, m = 1; j < 6; j++, m<<= 1)
//		{
//			if (!(cull & m))
//			{
//				float r = frustum[j][0] * dm->pos[0] +
//					      frustum[j][1] * dm->pos[1] +
//					      frustum[j][2] * dm->pos[2] +
//					      frustum[j][3];
//
//				//cull the diamond
//				if (std::pow(r, 2) > dm->bound_rad)
//				{
//					if (r < 0.0f) {
//						cull = CULL_OUT;
//					} else {
//						cull |= m; //IN
//					}
//				}
//				//else still overlaps this frustum plane
//			}
//		}
//	}
//
//    //if OUT state changes, update in/out listing on any draw tris
//	if ((dm->cull ^ cull) & CULL_OUT)
//	{
//		for (int j = 0; j < 2; j++)
//		{
//			if (dm->flags & (ROAM_TRI0 << j))
//			{
//				if (cull & CULL_OUT) {
//					RemoveTri(dm, j);
//				} else {
//					AddTri(dm, j);
//				}
//			}
//		}
//	}
//
//	//store the updated cull flags
//	dm->cull = cull;
//}
//
//void SplitMergeROAM::UpdatePriority(Diamond* dm)
//{
//	if (m_frame_count == dm->frame_count) {
//		return;
//	}
//	dm->frame_count = m_frame_count;
//
//	int k;
//	if ((dm->flags & ROAM_CLIPPED) ||
//		(dm->level >= m_max_level)) {
//		k = 0;
//	} else {
//		//set the local integer pointer (for the IEEE floating-point tricks)
//		float d;
//		int* ip = (int*)(&d);
//		d = dm->error_rad;
//		//set the local integer pointer (for the IEEE floating-point tricks)
//		k = *ip;
//		k += m_log2table[(k >> 15) & 0xff];
//
//		//distance calculation
//		auto& cam_pos = m_camera.GetPos();
//		d = std::pow((dm->pos[0] - cam_pos.x), 2) +
//			std::pow((dm->pos[1] - cam_pos.y), 2) +
//			std::pow((dm->pos[2] - cam_pos.z), 2);
//
//		//compute the fixed-point log_2 value (based on the distance to the camera)
//		int j = *ip;
//		j += m_log2table[(j >> 15) & 0xff];
//
//		//compute the fixed-point log_2(error/distance)
//		k = (k - j) + 0x10000000;
//
//		//scale and clamp the priority index to [1, IQMAX-1]
//		if (k<0)
//			k = 0;
//
//		k = (k >> 16) + 1;
//		if (k >= QUEUE_CAP)
//			k = QUEUE_CAP - 1;
//
//		//for OUT diamonds, reduce priority (but leave them ordered)
//		if (dm->cull & CULL_OUT)
//		{
//			if (k > m_size) {
//				k -= (m_size / 2);
//			} else {
//				k = (k + 1) >> 1;
//			}
//		}
//	}
//
//	Enqueue(dm, dm->flags & ROAM_ALLQ, k);
//}
//
//}