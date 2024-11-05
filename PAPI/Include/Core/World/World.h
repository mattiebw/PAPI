#pragma once

class Entity;

class World
{
public:
    void AddEntity(Ref<Entity> entity)
    {
        m_Entities.push_back(entity);
    }
    
    template<typename T, typename... Args>
    void EmplaceEntity(Args&&... args)
    {
        static_assert(std::is_base_of<Entity, T>::value, "T must inherit from Entity");
        m_Entities.emplace_back(CreateRef<T>(std::forward<Args>(args)...));
    }

    void Tick(double delta);

private:
    double m_TimeScale = 1.0f;
    double m_Delta = 0;
    std::vector<Ref<Entity>> m_Entities;
};
