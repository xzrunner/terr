#include "terr/TileMapTex.h"
#include "terr/TextureLoader.h"

#include <unirender/Blackboard.h>
#include <unirender/RenderContext.h>

namespace terr
{

TileMapTex::TileMapTex()
{
	for (int i = 0; i < MAX_TILE; ++i) {
		m_tile_map_tex[i] = nullptr;
	}
}

void TileMapTex::Init()
{
	m_tile_map_tex[LOWEST_TILE]  = TextureLoader::LoadFromFile("lowestTile.tga");
	m_tile_map_tex[LOW_TILE]     = TextureLoader::LoadFromFile("lowTile.tga");
	m_tile_map_tex[HIGH_TILE]    = TextureLoader::LoadFromFile("highTile.tga");
	m_tile_map_tex[HIGHEST_TILE] = TextureLoader::LoadFromFile("highestTile.tga");
}

void TileMapTex::Bind(int channel_offset) const
{
	auto& rc = ur::Blackboard::Instance()->GetRenderContext();
	for (int i = 0; i < MAX_TILE; ++i) {
		rc.BindTexture(m_tile_map_tex[i]->TexID(), channel_offset + i);
	}
}

}