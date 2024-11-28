#include "papipch.h"
#include "Game/Player.h"

#include "Core/Application.h"
#include "Core/Input/Input.h"
#include "Game/PulsatingRectangle.h"
#include "World/World.h"
#include "Render/Camera.h"
#include "Render/Font.h"
#include "Render/Renderer.h"
#include "Render/Viewport.h"

void Player::Created()
{
	m_Texture = CreateRef<Texture>("Content/Textures/jeremy.jpeg");

	m_Camera                            = CreateRef<Camera>();
	m_Camera->Transformation.Position.z = 3;

	m_Viewport = Application::GetRenderer()->CreateViewport();
	m_Viewport->SetCamera(m_Camera);
}

void Player::AddedToWorld(World *world)
{
	m_Viewport->SetWorld(Application::Get()->GetWorldFromPointer(m_World));

	for (int i = 0; i < 10; i++)
		world->AddEntity<PulsatingRectangle>();

	SetPosition(glm::vec3(-1, -1, 0));
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
	{
		input = normalize(input);
		FRect collision(EntityTransform.Position.x - 0.45f, EntityTransform.Position.y - 0.45f, 0.9f, 0.9f);
		collision.Position.x += input.x * static_cast<float>(delta) * 5.0f;
		if (m_World->RectOverlapsAnySolidTile(collision))
			collision.Position.x = EntityTransform.Position.x - 0.45f;
		collision.Position.y += input.y * static_cast<float>(delta) * 5.0f;
		if (m_World->RectOverlapsAnySolidTile(collision))
			collision.Position.y = EntityTransform.Position.y - 0.45f;
		EntityTransform.Position = glm::vec3(collision.Position.x + 0.45f, collision.Position.y + 0.45f,
		                                     EntityTransform.Position.z);
	}

	m_Camera->Transformation.Position.x = MathUtil::LerpSmooth(m_Camera->Transformation.Position.x,
	                                                           EntityTransform.Position.x, 0.001f,
	                                                           static_cast<float>(delta));
	m_Camera->Transformation.Position.y = MathUtil::LerpSmooth(m_Camera->Transformation.Position.y,
	                                                           EntityTransform.Position.y, 0.001f,
	                                                           static_cast<float>(delta));

	// if (Input::IsKeyDown(PAPI_KEY_Q))
	// 	m_Camera->Transformation.Rotation.y += delta * 5;
	// if (Input::IsKeyDown(PAPI_KEY_E))
	// 	m_Camera->Transformation.Rotation.y -= delta * 5;
}

void Player::Render()
{
	float sine = (glm::sin(m_Time) + 1) / 2;
	Application::GetQuadRenderer()->DrawQuad(EntityTransform.Position, glm::vec2(1.0f),
	                                         glm::vec4(sine, sine, sine, 1.0f), m_Texture);

	Transform tf = EntityTransform;
	tf.Position.y += 2;
	Application::GetTextRenderer().DrawString("Jerma", Font::GetDefaultFont(), tf.GetTransformationMatrix(), glm::vec4(1));
}
