﻿#pragma once

#include "Entity.h"

class Entity;

class World
{
public:
	void AddEntity(Ref<Entity> entity)
	{
		m_Entities[entity->GetUUID()] = entity;
	}

	template <typename T, typename... Args>
	void AddEntity(Args &&... args)
	{
		static_assert(std::is_base_of_v<Entity, T>, "T must inherit from Entity");
		UUID        uuid;
		Ref<Entity> entity = CreateRef<T>(std::forward<Args>(args)...);
		entity->SetUUID(uuid);
		entity->SetWorld(this);
		m_Entities[uuid] = entity;
	}

	void UpdateEntityUUID(UUID oldID, UUID newID);

	void Tick(double delta);
	void Render();

	NODISCARD FORCEINLINE double GetDelta() const { return m_Delta; }
	NODISCARD FORCEINLINE double GetUnscaledDelta() const { return m_UnscaledDelta; }
	NODISCARD FORCEINLINE double GetTimeScale() const { return m_TimeScale; }
	FORCEINLINE void             SetTimeScale(double timeScale)
	{
		m_TimeScale = timeScale;
		m_Delta     = m_UnscaledDelta * m_TimeScale;
	}

private:
	double                                m_TimeScale     = 1.0f;
	double                                m_Delta         = 0;
	double                                m_UnscaledDelta = 0;
	std::unordered_map<UUID, Ref<Entity>> m_Entities;
};