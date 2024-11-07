#include "papipch.h"
#include "World/Entity.h"

#include "World/World.h"

Entity::Entity()
{
}

Entity::Entity(std::string name, UUID uuid)
	: m_UUID(uuid), m_Name(std::move(name))
{
}

Entity::Entity(std::string name, const Transform &tf, UUID uuid)
	: EntityTransform(tf), m_UUID(uuid), m_Name(std::move(name))
{
}

Entity::Entity(std::string name, glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, UUID uuid)
	: EntityTransform(pos, rot, scale), m_UUID(uuid), m_Name(std::move(name))
{
}

Entity::~Entity()
{
}

void Entity::SetUUID(UUID uuid)
{
	if (m_World)
		m_World->UpdateEntityUUID(m_UUID, uuid);
	m_UUID = uuid;
}

void Entity::Tick(double delta)
{
	PAPI_INFO("Entity ticking, delta: {}", delta);
}

void Entity::Render()
{
	
}
