﻿#pragma once
#include "Core/Layer.h"

class World;
class Camera;

class PAPIGameLayer : public Layer
{
public:
    void OnAttach() override;
    void OnDetach() override;
    void Update(double delta) override;
    void Render(double delta) override;
    void RenderImGUI(double delta) override;

private:
    World* m_World;
    Camera* m_Camera;
};
