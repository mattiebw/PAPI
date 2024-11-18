#pragma once
#include "TileMapChunk.h"

#include "TileSet.h"

class TileMap
{
public:
	TileMap(const Ref<TileSet> &tileSet, int chunkWidth = 32, int chunkHeight = 32);

	void SetTile(int x, int y, uint32_t tile, bool canCreateChunk = false);
	NODISCARD uint32_t GetTile(int x, int y, bool canCreateChunk = true) const;

	NODISCARD FORCEINLINE uint32_t GetChunkWidth() const { return m_ChunkSize.x; };
	NODISCARD FORCEINLINE uint32_t GetChunkHeight() const { return m_ChunkSize.y; };
	NODISCARD FORCEINLINE const Ref<TileSet>& GetTileSet() const { return m_TileSet; };
	
	NODISCARD FORCEINLINE size_t GetLoadedChunkCount() const { return m_Chunks.size(); };

	NODISCARD TileMapChunk* GetChunk(glm::ivec2 index) { return m_Chunks[index].get(); }
	NODISCARD const TileMapChunk* GetChunk(glm::ivec2 index) const { return m_Chunks.at(index).get(); };
	NODISCARD TileMapChunk* GetChunkFromTileCoordinate(int x, int y, bool canCreateChunk = true);

	NODISCARD bool RectOverlapsSolidTile(const FRect& rect);

	void UpdateChunkLoading(const std::vector<glm::ivec2>& playerPositions);
	void Render();
	
	float Z = 0;
	
protected:
	// The key in our chunk map is the chunk coordinate.
	// For example, if the chunk size is 32x32 and the chunk coordinate is (1, 2),
	// then the top left tile in that chunk is tile (32, 64).
	std::unordered_map<glm::ivec2, Ref<TileMapChunk>> m_Chunks;
	glm::ivec2 m_ChunkSize;
	
	Ref<TileSet> m_TileSet;
};
