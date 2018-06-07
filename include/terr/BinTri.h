#pragma once

namespace terr
{

struct BinTri
{
	BinTri* left_child = nullptr;
	BinTri* right_child = nullptr;
	BinTri* base_neighbor = nullptr;
	BinTri* left_neighbor = nullptr;
	BinTri* right_neighbor = nullptr;
};

}