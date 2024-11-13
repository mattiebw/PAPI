#include "papipch.h"
#include "World/TileMap.h"

#include "Core/Application.h"

TileMap::TileMap(const Ref<SpriteSheet> &tileSet, int chunkWidth, int chunkHeight)
    : m_ChunkSize(chunkWidth, chunkHeight), m_TileSet(tileSet)
{
    m_Chunks[glm::ivec2(0, 0)] = CreateRef<TileMapChunk>(this, glm::ivec2(0, 0));
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
            glm::vec3(chunk.second->GetPosition(), 0.0f), chunk.second.get());
    }
}
