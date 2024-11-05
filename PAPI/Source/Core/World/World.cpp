#include "papipch.h"
#include "Core/World/World.h"

#include "Core/World/Entity.h"

void World::Tick(double delta)
{
    m_Delta = delta * m_TimeScale;
    for (Ref<Entity>& entity : m_Entities)
    {
        entity->Tick(m_Delta);
    }
}
