// code from http://vterrain.org/
// An implementation a terrain rendering engine
// based on the ideas and input of Seumas McNally.

#pragma once

#include "terr/BinTri.h"

#include <functional>

#define DEFAULT_POLYGON_TARGET	10000

namespace terr
{

struct BinTriPool;

class SplitOnlyROAM
{
public:
	SplitOnlyROAM(int size, BinTriPool& pool);
	~SplitOnlyROAM();

	void Init();
	bool Update();
	void Draw() const;

public:
	struct CallbackFuncs
	{
		std::function<uint8_t(int x, int y)> get_height = nullptr;
		std::function<float(int x, int y)> dis_to_camera = nullptr;
		std::function<bool(float x, float y, float radius)> sphere_in_frustum = nullptr;
		std::function<void(int x, int y)> send_vertex = nullptr;
	}; // CallbackFuncs

	void RegisterCallback(const CallbackFuncs& cb) { m_cb = cb; }

private:
	struct Pos
	{
		int x, y;

		Pos(int x, int y) : x(x), y(y) {}
	};

private:
	void Reset();

	// init
	void ComputeVariances();
	uint8_t ComputeTriangleVariance(int num, const Pos& v0, const Pos& v1, const Pos& va, int level);

	// update
	void AdjustQualityConstant();
	void SplitIfNeeded(int num, BinTri *tri, const Pos& v0, const Pos& v1, const Pos& va, bool entirely_in_frustum, int level);
	void Split(BinTri* tri);
	void Split2(BinTri* tri);

	// draw
	void RenderTri(BinTri* tri, const Pos& v0, const Pos& v1, const Pos& va) const;

private:
	CallbackFuncs m_cb;

	int m_size;

	BinTriPool& m_pool;

	BinTri* m_nw_tri = nullptr;
	BinTri* m_se_tri = nullptr;

	int m_levels;

	float m_quality_constant;

	int m_used_nodes;		// number of nodes needed for variance tree
	uint8_t* m_variance;	// the variance implicit binary tree

	float* m_hypo_len;		// table of hypotenuse lengths

	int m_split_cutoff;

	//// stat
	//int m_drawn_tris;

}; // SplitOnlyROAM

}