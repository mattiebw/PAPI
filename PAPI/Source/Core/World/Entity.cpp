#include "papipch.h"
#include "Core/World/Entity.h"

Entity::Entity()
{
}

Entity::Entity(std::string name)
	: m_Name(std::move(name))
{
}

Entity::Entity(std::string name, const Transform &tf)
	: EntityTransform(tf), m_Name(std::move(name))
{
}

Entity::Entity(std::string name, glm::vec3 pos, glm::vec3 rot, glm::vec3 scale)
	: EntityTransform(pos, rot, scale), m_Name(std::move(name))
{
}

Entity::~Entity()
{
}

void Entity::Tick(double delta)
{
}
