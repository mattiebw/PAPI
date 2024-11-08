#include "papipch.h"
#include "Game/Player.h"

#include "Core/Application.h"
#include "Render/Renderer.h"

void Player::Created()
{
	m_Texture = CreateRef<Texture>("Content/jeremy.jpeg");
}

void Player::Tick(double delta)
{
	m_Time += delta;
}

void Player::Render()
{
	Application::GetRenderer()->GetQuadRenderer()->DrawQuad(glm::vec3(0.0f), glm::vec2(1.5f + glm::sin(m_Time), 0.5f), glm::vec4(0.8f, 0.2f, 0.2f, 1.0f));
	Application::GetRenderer()->GetQuadRenderer()->DrawQuad(glm::vec3(0.0f, 2.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec4(1.0f), m_Texture);
}
