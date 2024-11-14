#include "papipch.h"
#include "World/TileMap.h"

#include "Core/Application.h"

TileMap::TileMap(const Ref<TileSet> &tileSet, int chunkWidth, int chunkHeight)
    : m_ChunkSize(chunkWidth, chunkHeight), m_TileSet(tileSet)
{
    m_Chunks[glm::ivec2(0, 0)] = CreateRef<TileMapChunk>(this, glm::ivec2(0, 0));
}

void TileMap::SetTile(int x, int y, uint32_t tile, bool canCreateChunk)
{
    glm::ivec2 tileCoords = glm::ivec2(std::floor(x / m_ChunkSize.x), std::floor(y / m_ChunkSize.y));
    auto chunk = m_Chunks.find(tileCoords);
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
    auto chunk = m_Chunks.find(tileCoords);
    if (chunk == m_Chunks.end())
    {
        if (canCreateChunk)
        {
            // MW @todo: Create the chunk
            return 0;
        }
        
        return static_cast<uint32_t>(-1);
    }
    else
        return chunk->second->GetTile(x % m_ChunkSize.x, y % m_ChunkSize.y);
}

void TileMap::UpdateChunkLoading(const std::vector<glm::ivec2> &playerPositions)
{
    
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
