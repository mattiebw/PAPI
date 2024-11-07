#pragma once

class World;

class Entity
{
public:
	Entity();
	Entity(std::string name, UUID uuid = UUID());
	Entity(std::string name, const Transform &tf, UUID uuid = UUID());
	Entity(std::string name, glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, UUID uuid = UUID());
	virtual ~Entity();

	Entity(const Entity &other)                = delete;
	Entity(Entity &&other) noexcept            = delete;
	Entity& operator=(const Entity &other)     = delete;
	Entity& operator=(Entity &&other) noexcept = delete;

	NODISCARD FORCEINLINE const std::string& GetName() const { return m_Name; }
	NODISCARD FORCEINLINE const glm::vec3&   GetPosition() const { return EntityTransform.Position; }
	NODISCARD FORCEINLINE const glm::vec3&   GetRotation() const { return EntityTransform.Rotation; }
	NODISCARD FORCEINLINE const glm::vec3&   GetScale() const { return EntityTransform.Scale; }
	NODISCARD FORCEINLINE World*             GetWorld() const { return m_World; }
	NODISCARD FORCEINLINE UUID               GetUUID() const { return m_UUID; }

	FORCEINLINE void SetName(const std::string &name) { m_Name = name; }
	FORCEINLINE void SetPosition(glm::vec3 newPos) { EntityTransform.Position = newPos; }
	FORCEINLINE void SetRotation(glm::vec3 newRot) { EntityTransform.Rotation = newRot; }
	FORCEINLINE void SetScale(glm::vec3 newScale) { EntityTransform.Scale = newScale; }
	FORCEINLINE void SetWorld(World *world) { m_World = world; }

	void SetUUID(UUID uuid);

	virtual void Tick(double delta);
	virtual void Render();

	Transform EntityTransform;

	static constexpr uint32_t s_ID = crc32("hello");

private:
	UUID        m_UUID;
	std::string m_Name  = "Entity";
	World *     m_World = nullptr;

	friend class World;
};
