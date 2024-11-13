#include "papipch.h"
#include "World/World.h"

#include <ranges>

#include "Game/Player.h"
#include "Render/SpriteSheet.h"
#include "Render/Texture.h"
#include "World/TileMap.h"
#include "World/Entity.h"

World::World()
{
	TextureSpecification spec;
	spec.MagFilter                  = FilterMode::Nearest;
	spec.MinFilter                  = FilterMode::Nearest;
	spec.Wrap                       = WrapMode::ClampToEdge;
	spec.FlipVertically             = false;
	Ref<Texture> spritesheetTexture = CreateRef<Texture>("Content/Textures/TerrainSpritesheet.png", spec);

	Ref<SpriteSheet> spritesheet = CreateRef<SpriteSheet>(spritesheetTexture);
	spritesheet->CreateTilesFromTileSize(16, 16);
	m_TileMap = CreateRef<TileMap>(spritesheet);
	m_TileMap->Z = -10;
}

void World::UpdateEntityUUID(UUID oldID, UUID newID)
{
	PAPI_ASSERT(m_Entities.contains(oldID) && "Entity does not exist");
	PAPI_ASSERT(!m_Entities.contains(newID) && "Entity with new ID already exists");

	Ref<Entity> entity = m_Entities[oldID];
	m_Entities.erase(oldID);
	m_Entities[newID] = entity;
}

void World::DestroyEntity(UUID id)
{
	auto it = m_Entities.find(id);

	if (it == m_Entities.end())
	{
		PAPI_ERROR("Entity with ID {} does not exist", id);
		return;
	}

	it->second->Destroyed();
	m_Entities.erase(it);
}

void World::DestroyEntity(Entity *entity)
{
	DestroyEntity(entity->GetUUID());
}

void World::Tick(double delta)
{
	m_UnscaledDelta = delta;
	m_Delta         = delta * m_TimeScale;
	std::vector<glm::ivec2> playerPositions;
	for (const auto &entity : m_Entities | std::views::values)
	{
		entity->Tick(m_Delta);

		if (entity->GetTypeID() == Player::s_EntityTypeID)
		{
			Player *player = static_cast<Player*>(entity.get());
			playerPositions.push_back(player->GetPosition());
		}
	}
	m_TileMap->UpdateChunkLoading(playerPositions);
}

void World::Render()
{
	m_TileMap->Render();

	for (const auto &entity : m_Entities | std::views::values)
	{
		entity->Render();
	}
}

void World::Clean()
{
	m_Entities.clear();
}
