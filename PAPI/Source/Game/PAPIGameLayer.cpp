﻿#include "papipch.h"
#include "Game/PAPIGameLayer.h"

#include "Core/Application.h"
#include "Core/Input/Input.h"
#include "Core/Input/Scancode.h"
#include "Game/Player.h"
#include "Render/Renderer.h"
#include "Render/Viewport.h"
#include "Render/Camera.h"
#include "Render/Font.h"
#include "World/TileSets.h"
#include "World/World.h"

void PAPIGameLayer::OnAttach()
{
	PAPI_INFO("Initialising PAPI game!");

	// Bodging some stuff!
	TileSets::Init();
	Application *app   = Application::Get();
	Ref<World>   world = app->AddWorld();
	world->AddEntity<Player>("Player");

	m_TextTransform.Position.z = -0.05f;
}

void PAPIGameLayer::OnDetach()
{
}

void PAPIGameLayer::Update(double delta)
{
}

void PAPIGameLayer::Render(double delta)
{
	Application::GetTextRenderer().DrawString("Hello", Font::GetDefaultFont(), m_TextTransform.GetTransformationMatrix(), glm::vec4(1.0f));
}

void PAPIGameLayer::RenderImGUI(double delta)
{
	ImGui::Begin("Text Test");
	ImGui::InputFloat3("Text Position", glm::value_ptr(m_TextTransform.Position));
	ImGui::InputFloat3("Text Rotation", glm::value_ptr(m_TextTransform.Rotation));
	ImGui::InputFloat3("Text Scale", glm::value_ptr(m_TextTransform.Scale));
	ImGui::End();
}
