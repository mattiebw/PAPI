#include "papipch.h"
#include "World/TileMapChunk.h"

#include "Core/Application.h"
#include "Render/Renderer.h"
#include "Render/SpriteSheet.h"
#include "Render/VertexArray.h"
#include "World/TileMap.h"

TileMapChunk::TileMapChunk()
	: m_Position(0), m_Size(0), m_TileMap(nullptr)
{
}

TileMapChunk::TileMapChunk(TileMap *tileMap, glm::ivec2 position, glm::ivec2 size)
	: m_Position(position), m_Size(size), m_TileMap(tileMap)
{
	m_Tiles = new uint32_t[m_Size.x * m_Size.y];
	m_TileData = new glm::vec2[m_Size.x * m_Size.y * 4];
	memset(m_Tiles, 0, sizeof(uint32_t) * m_Size.x * m_Size.y);
	memset(m_TileData, 0, sizeof(uint32_t) * m_Size.x * m_Size.y * 4);

	// Our tile data buffer is just a contiguous buffer of vec2s, representing the texture coordinates of the tiles.
	// 4 vec2s per tile.
	m_TileDataBuffer = CreateRef<VertexBuffer>(static_cast<uint32_t>(sizeof(glm::vec2) * m_Size.x * m_Size.y * 4));
	m_TileDataBuffer->SetLayout(BufferLayout({
			{"a_TexCoord", ShaderDataType::Float2},
	}));
	
	m_VertexArray = CreateRef<VertexArray>();
	m_VertexArray->AddVertexBuffer(Application::GetRenderer()->GetQuadVertexBuffer());
	m_VertexArray->AddVertexBuffer(m_TileDataBuffer);
	m_VertexArray->SetIndexBuffer(Application::GetRenderer()->GetQuadIndexBuffer());
}

TileMapChunk::~TileMapChunk()
{
	delete[] m_Tiles;
	delete[] m_TileData;
}

void TileMapChunk::SetTile(int x, int y, uint32_t tile)
{
	int index = y * m_Size.x + x;
	m_Tiles[index] = tile;
	m_DataDirty = true;
}

void TileMapChunk::UpdateTileData()
{
	if (!m_DataDirty)
		return;
	
	for (int i = 0; i < m_Size.x * m_Size.y; i++)
	{
		SpriteSheetSprite tile = m_TileMap->GetTileSet()->GetSprite(m_Tiles[i]);
		m_TileData[i * 4 + 0] = tile.TexCoordsMin;
		m_TileData[i * 4 + 1] = glm::vec2(tile.TexCoordsMax.x, tile.TexCoordsMin.y);
		m_TileData[i * 4 + 2] = tile.TexCoordsMax;
		m_TileData[i * 4 + 3] = glm::vec2(tile.TexCoordsMin.x, tile.TexCoordsMax.y);
	}

	m_TileDataBuffer->SetData(m_TileData, static_cast<uint32_t>(m_Size.x * m_Size.y * 4));
	m_DataDirty = false;
}
