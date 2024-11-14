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
	m_TileShaderData = new TileShaderData[m_Size.x * m_Size.y];
	memset(m_Tiles, 0, sizeof(uint32_t) * m_Size.x * m_Size.y);
	memset(m_TileShaderData, 0, sizeof(TileShaderData) * m_Size.x * m_Size.y);

	// Our tile data buffer is just a contiguous buffer of vec2s, representing the top left texture coordinates of the tiles.
	m_TileDataBuffer = CreateRef<VertexBuffer>(static_cast<uint32_t>(sizeof(TileShaderData) * m_Size.x * m_Size.y));
	m_TileDataBuffer->SetLayout(BufferLayout({
			{"a_TexCoordTopLeft", ShaderDataType::Float2, 0, 1 },
			{"a_Rot", ShaderDataType::Float, 0, 1 },
	}));
	
	m_VertexArray = CreateRef<VertexArray>();
	m_VertexArray->AddVertexBuffer(Application::GetRenderer()->GetTileQuadVertexBuffer());
	m_VertexArray->AddVertexBuffer(m_TileDataBuffer);
	m_VertexArray->SetIndexBuffer(Application::GetRenderer()->GetTileQuadIndexBuffer());
}

TileMapChunk::~TileMapChunk()
{
	delete[] m_Tiles;
	delete[] m_TileShaderData;
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
		SpriteSheetSprite tile = m_TileMap->GetTileSet()->GetSpriteForTile(m_Tiles[i]);
		m_TileShaderData[i].TopLeftTexCoord = tile.TexCoordsMin;
		m_TileShaderData[i].Rot = 0;
	}

	m_TileDataBuffer->SetData(m_TileShaderData, static_cast<uint32_t>(m_Size.x * m_Size.y * sizeof(TileShaderData)));
	m_DataDirty = false;
}
