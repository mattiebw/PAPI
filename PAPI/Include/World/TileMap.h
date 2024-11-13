#pragma once
#include "TileMapChunk.h"

class SpriteSheet;

class TileMap
{
public:
	TileMap(const Ref<SpriteSheet> &tileSet);

	void SetTile(int x, int y, uint32_t tile);
	uint32_t GetTile(int x, int y) const;

	void Resize(uint32_t width, uint32_t height);

	uint32_t GetWidth() const;
	uint32_t GetHeight() const;

	uint32_t GetChunkWidth() const;
	uint32_t GetChunkHeight() const;

	uint32_t GetChunkCount() const;

	TileMapChunk& GetChunk(uint32_t index);
	const TileMapChunk& GetChunk(uint32_t index) const;

	const Ref<SpriteSheet>& GetTileSet() const;
	
protected:
	std::vector<TileMapChunk> m_Chunks;
	Ref<SpriteSheet> m_TileSet;
};
