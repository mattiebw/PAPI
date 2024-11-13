#include "papipch.h"
#include "World/TileMapChunk.h"

TileMapChunk::TileMapChunk(TileMap *tileMap, glm::ivec2 position, glm::ivec2 size)
	: m_Position(position), m_Size(size), m_TileMap(tileMap)
{
	m_Tiles = new uint32_t[m_Size.x * m_Size.y];
}

TileMapChunk::~TileMapChunk()
{
	delete[] m_Tiles;
}

void TileMapChunk::SetTile(int x, int y, uint32_t tile) const
{
	int index = y * m_Size.x + x;
	m_Tiles[index] = tile;
}
