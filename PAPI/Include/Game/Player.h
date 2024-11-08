#pragma once

#include "Render/Texture.h"
#include "World/Entity.h"

class Player : public Entity
{
    ENTITY_CLASS_BODY(Player);

    void Created() override;
    void Tick(double delta) override;
    void Render() override;

private:
    float m_Time = 0;
    Ref<Texture> m_Texture;
};
