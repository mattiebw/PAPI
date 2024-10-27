#include "papipch.h"
#include "Render/Renderer.h"

#include "Core/Application.h"
#include "Core/Window.h"

Renderer::~Renderer()
{
	Shutdown();
}

bool Renderer::Init(Ref<Window> window)
{
	PAPI_INFO("Initialising renderer");

	m_Window      = window;
	m_Initialised = true;

	if (!InitOpenGL())
		return false;

	return true;
}

bool Renderer::InitOpenGL()
{
	static bool glInitialised = false;
	if (glInitialised)
		return true;

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	PAPI_ASSERT(m_Window && "Window should be set");
	m_Context = m_Window->GetContext();

	int version = gladLoadGL(SDL_GL_GetProcAddress);
	if (version == 0)
	{
		const char *error = "Failed to initialise OpenGL with GLAD";
		PAPI_ERROR("{}", error);
		Application::Get()->ShowError(error, "OpenGL Error");
		return false;
	}

	glInitialised = true;
	PAPI_INFO("Initialised OpenGL v{}.{}", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));
	PAPI_INFO("   OpenGL Vendor: {}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
	PAPI_INFO("   OpenGL Renderer: {}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));

	return true;
}

void Renderer::Shutdown()
{
	if (!m_Initialised)
		return;
	PAPI_INFO("Shutting down renderer");
	m_Initialised = false;
}
