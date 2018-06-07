// code from http://www.cognigraph.com/ROAM_homepage/
//
//#pragma once
//
//#include <painting3/Camera.h>
//#include <painting3/ViewFrustum.h>
//
//#include <functional>
//
//namespace terr
//{
//
//class SplitMergeROAM
//{
//public:
//	//+..................................................................+
//	//+                                                                  +
//	//+                                *                                 +
//	//+                               / \                                +
//	//+                              /   \                               +
//	//+                             /     \                              +
//	//+                            o       o                             +
//	//+                           / \     / \                            +
//	//+                          /   \   /   \                           +
//	//+                         /     \ /     \                          +
//	//+                        o...x...a0..w...o                         +
//	//+                       ..\  .  /|\  .  /..                        +
//	//+                      . . \ .k0 | k3. / . .                       +
//	//+                     .  .  \./  |  \./  .  .                      +
//	//+                    .   .  p0...c...p1  .   .                     +
//	//+                   .    .  ..\  |  /..  .    .                    +
//	//+                  .     . . .k1 | k2. . .     .                   +
//	//+                 .      ..  .  \|/  .  ..      .                  +
//	//+                .       ....y..a1...z....       .                 +
//	//+                 .       .     . .     .       .                  +
//	//+                  .       .   .   .   .       .                   +
//	//+                   .       . .     . .       .                    +
//	//+                    .       .       .       .                     +
//	//+                     .     .         .     .                      +
//	//+                      .   .           .   .                       +
//	//+                       . .             . .                        +
//	//+                        .               .                         +
//	//+                                                                  +
//	//+..................................................................+
//	//
//	// c -- the center diamond referencing all its relatives
//	// p0 -- left parent
//	// p1 -- right parent
//	// a0 -- quadtree parent (up ancestor)
//	// a1 -- down ancestor
//	// k0 -- upper-left kid
//	// k1 -- lower-left kid
//	// k2 -- lower-right kid
//	// k3 -- upper-right kid
//	// x,y,z,w  -- opposite parent with respect to kids 0,1,2,3
//	struct Diamond
//	{
//		Diamond* p[2];		// left and right parent
//		Diamond* a[2];		// up and down ancestor
//		Diamond* k[4];		// kids
//
//		// in queue
//		Diamond *prev, *next;
//
//		float pos[3];       // vertex position
//		float bound_rad;	// radius of sphere bound squared (-1 if NEW)
//		float error_rad;	// radius of pointwise error squared
//
//		int ref_count;
//
//		unsigned short queue_idx;
//		unsigned short tri_idx[2];
//
//		uint8_t i[2];		// kid index within each of parents
//
//		int8_t level;
//
//		uint8_t cull;
//		uint8_t flags;
//		uint8_t split_flags;
//
//		uint8_t frame_count;
//	};
//
//	SplitMergeROAM(int max_level, int pool_size, int size,
//		const pt3::Camera& cam, const pt3::ViewFrustum& vf,
//		std::function<float(float, float)> get_height_cb);
//	~SplitMergeROAM();
//
//	void Update();
//
//private:
//	Diamond* NewDiamond();
//
//	void AddRef(Diamond* dm);
//	void RemoveRef(Diamond* dm);
//
//	void AllocateTri(Diamond* dm, int j);
//	void FreeTri(Diamond* dm, int j);
//	void AddTri(Diamond* dm, int j);
//	void RemoveTri(Diamond* dm, int j);
//
//	Diamond* GetKid(Diamond* dm, int kid_idx);
//
//	void Enqueue(Diamond* dm, char queue_flag, int new_idx);
//
//	void Split(Diamond* dm);
//	void Merge(Diamond* dm);
//
//	void UpdateKidsCull(Diamond* dm);
//	void UpdateCull(Diamond* dm);
//	void UpdatePriority(Diamond* dm);
//
//private:
//	static const size_t QUEUE_CAP = 4096;
//	static const size_t TRI_IMAX  = 65536;
//
//private:
//	// flags
//	static const uint8_t ROAM_SPLIT   = 0x01;
//	static const uint8_t ROAM_TRI0    = 0x04;
//	static const uint8_t ROAM_TRI1    = 0x08;
//	static const uint8_t ROAM_CLIPPED = 0x10;
//	static const uint8_t ROAM_SPLITQ  = 0x40;
//	static const uint8_t ROAM_MERGEQ  = 0x80;
//	static const uint8_t ROAM_ALLQ    = 0xc0;
//	static const uint8_t ROAM_UNQ     = 0x00;
//
//	// cull
//	static const uint8_t CULL_ALLIN   = 0x3f;
//	static const uint8_t CULL_OUT     = 0x40;
//
//	// split
//	static const uint8_t SPLIT_K0     = 0x01;
//	static const uint8_t SPLIT_K      = 0x0f;
//	static const uint8_t SPLIT_P0     = 0x10;
//	static const uint8_t SPLIT_P      = 0x30;
//
//private:
//	const pt3::Camera&      m_camera;
//	const pt3::ViewFrustum& m_frustum;
//
//	std::function<float(float, float)> m_get_height_cb;
//
//	Diamond* m_pool;
//	int m_pool_size;
//	Diamond *m_pool_begin, *m_pool_end;
//	int m_free_count;			// number of elements on free list
//
//	float* m_level_md_size;		// max midpoint displacement per level
//	int m_max_level;
//
//	Diamond* m_root;
//
//	Diamond* m_split_queue[QUEUE_CAP];		//split priority queue
//	Diamond* m_merge_queue[QUEUE_CAP];		//merge priority queue
//	int m_queue_min, m_queue_max;			//min/max occupied bucket
//	int m_queue_coarse;						// coarseness limit on priority index
//
//	////most recently unlocked diamonds
//	//Diamond* free_dm[2];
//
//	uint8_t m_frame_count;		// current frame count
//
//	int m_log2table[256];		//correction to float->int conversions
//
//	int* m_tri_dm_ij;			//packed diamond index and side
//	int m_max_tris;				// target triangle count
//	int m_max_tri_chunks;		//max number of tri chunks allowed
//	int m_free_tri;				//first free tri chunk
//	int m_free_tri_count;		//number of triangles on free list
//
//	float* m_vert_buf;			//data attachment specific to a library step
//
//	int m_size;
//
//	// stat
//	int m_verts_per_frame;
//	int m_tris_per_frame;
//
//}; // SplitMergeROAM
//
//}