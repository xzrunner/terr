#include "terraingraph/TerrainGraph.h"

namespace terraingraph
{

CU_SINGLETON_DEFINITION(TerrainGraph);

extern void regist_rttr();

TerrainGraph::TerrainGraph()
{
	regist_rttr();
}

}