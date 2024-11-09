#pragma once

#include "Render/Texture.h"
#include "World/Entity.h"

class Viewport;
class Camera;

class Player : public Entity
{
    ENTITY_CLASS_BODY(Player);

    void Created() override;
    void AddedToWorld(World *world) override;
    void Tick(double delta) override;
    void Render() override;

private:
    float m_Time = 0;
    Ref<Texture> m_Texture;
    Ref<Viewport> m_Viewport;
    Ref<Camera> m_Camera;
};
