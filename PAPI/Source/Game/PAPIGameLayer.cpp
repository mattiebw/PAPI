#include "papipch.h"
#include "Game/PAPIGameLayer.h"

#include "Core/Application.h"
#include "Game/Player.h"
#include "Render/Renderer.h"
#include "Render/Viewport.h"
#include "Render/Camera.h"
#include "World/World.h"

void PAPIGameLayer::OnAttach()
{
    PAPI_INFO("Initialising PAPI game!");

    // Bodging some stuff!
    Application *app = Application::Get();
    Ref<World> world = app->AddWorld();
    m_World = world.get();
    auto viewport = Application::GetRenderer()->CreateViewport();
    viewport->SetWorld(world);
    auto cam = CreateRef<Camera>();
    cam->Transformation.Position = { 0, 1.1f, 1.0f };
    viewport->SetCamera(cam);
    m_Camera = viewport->GetCamera().get();
    world->AddEntity<Player>("Player");
}

void PAPIGameLayer::OnDetach()
{
    
}

void PAPIGameLayer::Update(double delta)
{
}

void PAPIGameLayer::Render(double delta)
{
}

void PAPIGameLayer::RenderImGUI(double delta)
{
    ImGui::Begin("Properties");
    ImGui::DragFloat3("Camera Position", &m_Camera->Transformation.Position.x, 0.1f);
    ImGui::InputDouble("World Timescale", m_World->GetTimeScaleRef());
    ImGui::End();
}
