#include "papipch.h"
#include "World/World.h"

#include <ranges>

#include "World/Entity.h"

void World::UpdateEntityUUID(UUID oldID, UUID newID)
{
	PAPI_ASSERT(m_Entities.contains(oldID) && "Entity does not exist");
	PAPI_ASSERT(!m_Entities.contains(newID) && "Entity with new ID already exists");
		
	auto entity = m_Entities[oldID];
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
	m_Delta = delta * m_TimeScale;
	for (const auto &entity : m_Entities | std::views::values)
	{
		entity->Tick(m_Delta);
	}
}

void World::Render()
{
	for (const auto &entity : m_Entities | std::views::values)
	{
		entity->Render();
	}
}

void World::Clean()
{
	m_Entities.clear();
}
