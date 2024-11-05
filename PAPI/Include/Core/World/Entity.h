#pragma once

class World;

class Entity
{
public:
    Entity();
    Entity(std::string name);
    Entity(std::string name, const Transform& tf);
    Entity(std::string name, glm::vec3 pos, glm::vec3 rot, glm::vec3 scale);
    ~Entity();

    NODISCARD FORCEINLINE const std::string& GetName() const { return m_Name; }
    NODISCARD FORCEINLINE glm::vec3 GetPosition() const { return EntityTransform.Position; }
    NODISCARD FORCEINLINE glm::vec3 GetRotation() const { return EntityTransform.Rotation; }
    NODISCARD FORCEINLINE glm::vec3 GetScale() const { return EntityTransform.Scale; }

    FORCEINLINE void SetName(const std::string &name) { m_Name = name; }
    FORCEINLINE void SetPosition(glm::vec3 newPos) { EntityTransform.Position = newPos; }
    FORCEINLINE void SetRotation(glm::vec3 newRot) { EntityTransform.Rotation = newRot; }
    FORCEINLINE void SetScale(glm::vec3 newScale) { EntityTransform.Scale = newScale; }

    void Tick(double delta);

    Transform EntityTransform;

private:
    std::string m_Name = "Entity";
    World* m_World = nullptr;

    friend class World;
};
