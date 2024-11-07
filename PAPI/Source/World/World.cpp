#include "papipch.h"
#include "World/World.h"

#include "World/Entity.h"

void World::UpdateEntityUUID(UUID oldID, UUID newID)
{
	PAPI_ASSERT(m_Entities.contains(oldID) && "Entity does not exist");
	PAPI_ASSERT(!m_Entities.contains(newID) && "Entity with new ID already exists");
		
	auto entity = m_Entities[oldID];
	m_Entities.erase(oldID);
	m_Entities[newID] = entity;
}

void World::Tick(double delta)
{
	m_UnscaledDelta = delta;
	m_Delta = delta * m_TimeScale;
	for (const auto &[_, entity] : m_Entities)
	{
		entity->Tick(m_Delta);
	}
}

void World::Render()
{
	for (const auto &[_, entity] : m_Entities)
	{
		entity->Render();
	}
}
