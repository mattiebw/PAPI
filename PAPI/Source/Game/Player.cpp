﻿#include "papipch.h"
#include "Game/Player.h"

#include "Core/Application.h"
#include "Core/Input/Input.h"
#include "Game/PulsatingRectangle.h"
#include "World/World.h"
#include "Render/Camera.h"
#include "Render/Renderer.h"
#include "Render/Viewport.h"

void Player::Created()
{
	m_Texture = CreateRef<Texture>("Content/jeremy.jpeg");

	m_Camera                            = CreateRef<Camera>();
	m_Camera->Transformation.Position.z = 3;

	m_Viewport = Application::GetRenderer()->CreateViewport();
	m_Viewport->SetCamera(m_Camera);
}

void Player::AddedToWorld(World *world)
{
	m_Viewport->SetWorld(Application::Get()->GetWorldFromPointer(m_World));

	for (int i = 0; i < 30000; i++)
		world->AddEntity<PulsatingRectangle>();
}

void Player::Tick(double delta)
{
	m_Time += static_cast<float>(delta);

	glm::vec2 input(0.0f);
	input.x -= Input::IsKeyDown(PAPI_KEY_A) ? 1.0f : 0.0f;
	input.x += Input::IsKeyDown(PAPI_KEY_D) ? 1.0f : 0.0f;
	input.y -= Input::IsKeyDown(PAPI_KEY_S) ? 1.0f : 0.0f;
	input.y += Input::IsKeyDown(PAPI_KEY_W) ? 1.0f : 0.0f;

	if (input.x != 0 || input.y != 0)
		input = normalize(input);
	EntityTransform.Position += glm::vec3(input * 5.0f * static_cast<float>(delta), 0.0f);

	m_Camera->Transformation.Position.x = EntityTransform.Position.x;
	m_Camera->Transformation.Position.y = EntityTransform.Position.y;
}

void Player::Render()
{
	float sine = (glm::sin(m_Time) + 1) / 2;
	Application::GetRenderer()->GetQuadRenderer()->DrawQuad(EntityTransform.Position, glm::vec2(1.0f),
	                                                        glm::vec4(sine, sine, sine, 1.0f), m_Texture);
}
