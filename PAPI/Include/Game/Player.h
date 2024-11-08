#pragma once

#include "Render/Texture.h"
#include "World/Entity.h"

class Player : public Entity
{
    ENTITY_CLASS_BODY(Player);

    void Created() override;
    void Render() override;

private:
    Ref<Texture> m_Texture;
};
