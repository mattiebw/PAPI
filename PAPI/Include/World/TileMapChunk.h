#pragma once

class TileMap;

class TileMapChunk
{
public:
	TileMapChunk(TileMap *tileMap, glm::ivec2 position, glm::ivec2 size = {32, 32});
	~TileMapChunk();

	void SetTile(int x, int y, uint32_t tile) const;

protected:
	// Position is the top-left corner of the chunk in world space. Size is the size of the chunk in tiles.
	glm::ivec2 m_Position, m_Size;
	uint32_t *m_Tiles;

	TileMap *m_TileMap;

	friend class Renderer;
};
