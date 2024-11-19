#include "papipch.h"
#include "Game/PAPIGameLayer.h"

#include "Core/Application.h"
#include "Core/Input/Input.h"
#include "Core/Input/Scancode.h"
#include "Game/Player.h"
#include "Render/Renderer.h"
#include "Render/Viewport.h"
#include "Render/Camera.h"
#include "World/World.h"

void PAPIGameLayer::OnAttach()
{
	PAPI_INFO("Initialising PAPI game!");

	// Bodging some stuff!
	Application *app   = Application::Get();
	Ref<World>   world = app->AddWorld();
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
}
