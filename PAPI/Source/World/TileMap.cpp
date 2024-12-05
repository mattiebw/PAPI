#include "papipch.h"
#include "World/TileMap.h"

#include "Core/Application.h"

TileMap::TileMap(const Ref<TileSet> &tileSet, int chunkWidth, int chunkHeight)
	: m_ChunkSize(chunkWidth, chunkHeight), m_TileSet(tileSet)
{
	m_ChunkProvider            = CreateRef<DefaultChunkProvider>();
}

void TileMap::SetTile(int x, int y, uint32_t tile, bool canCreateChunk)
{
	glm::ivec2 tileCoords = glm::ivec2(std::floor(x / m_ChunkSize.x), std::floor(y / m_ChunkSize.y));
	auto       chunk      = m_Chunks.find(tileCoords);
	if (chunk == m_Chunks.end())
	{
		if (canCreateChunk)
		{
			// MW @todo: Create the chunk
		}
	}
	else
		chunk->second->SetTile(x % m_ChunkSize.x, y % m_ChunkSize.y, tile);
}

uint32_t TileMap::GetTile(int x, int y, bool canCreateChunk) const
{
	glm::ivec2 tileCoords = glm::ivec2(std::floor(x / m_ChunkSize.x), std::floor(y / m_ChunkSize.y));
	auto       chunk      = m_Chunks.find(tileCoords);
	if (chunk == m_Chunks.end())
	{
		if (canCreateChunk)
		{
			// MW @todo: Create the chunk
			return 0;
		}

		return static_cast<uint32_t>(-1);
	}
	return chunk->second->GetTile(x % m_ChunkSize.x, y % m_ChunkSize.y);
}

void TileMap::UpdateChunkLoading(const std::vector<glm::ivec2> &playerPositions)
{
	for (glm::ivec2 pos : playerPositions)
	{
		// TODO: Fill this in!
	}
}

void TileMap::Render()
{
	for (auto chunk : m_Chunks)
	{
		// MW @todo: Simple bounds culling
		Application::GetRenderer()->GetTilemapRenderer().DrawTileMapChunk(
			glm::vec3(chunk.second->GetPosition(), Z), chunk.second.get());
	}
}

TileMapChunk* TileMap::GetChunkFromTileCoordinate(int x, int y, bool canCreateChunk)
{
	glm::ivec2 tileCoords = glm::ivec2(std::floor(static_cast<float>(x) / m_ChunkSize.x),
	                                   std::floor(static_cast<float>(y) / m_ChunkSize.y));
	auto chunk = m_Chunks.find(tileCoords);
	if (chunk == m_Chunks.end())
	{
		// MW @todo: Create the chunk.
		if (canCreateChunk)
		{
		}
		return nullptr;
	}
	return chunk->second.get();
}

void TileMap::SetChunkProvider(const Ref<ChunkProvider> &chunkProvider)
{
	m_ChunkProvider = chunkProvider;
}

bool TileMap::RectOverlapsSolidTile(const FRect &rect)
{
	for (int y = static_cast<int>(std::floor(rect.Position.y)); y < static_cast<int>(std::ceil(
		     rect.Position.y + rect.Size.y)); y++)
	{
		for (int x = static_cast<int>(std::floor(rect.Position.x)); x < static_cast<int>(std::ceil(
			     rect.Position.x + rect.Size.x)); x++)
		{
			// MW @todo @perf: Optimise! We shouldn't be looking up the chunk for every single tile.
			// We should try and check each chunk we overlap one by one first.
			// std::vector<glm::ivec2> GetOverlappedChunks(Rect)?

			TileMapChunk *chunk = GetChunkFromTileCoordinate(x, y, false);
			if (chunk != nullptr)
			{
				int cx = x % chunk->GetSize().x;
				if (cx < 0) cx = chunk->GetSize().x + cx;
				int cy = y % chunk->GetSize().y;
				if (cy < 0) cy = chunk->GetSize().y + cy;
				if (chunk->GetTileDataForTile(cx, cy).IsSolid)
					return true;
			}
		}
	}

	return false;
}
