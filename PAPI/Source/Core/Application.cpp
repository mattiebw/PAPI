#include "papipch.h"
#include "Core/Application.h"

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_messagebox.h>

Application::Application(const ApplicationSpecification& spec)
{
	m_Specification = spec;
}

bool Application::Init()
{
	if (!InitSDL())
		return false;

	return true;
}

void Application::Run()
{
	m_Running = true;
	while (m_Running)
	{
		m_Running = false;
	}
}

void Application::Shutdown()
{
	ShutdownSDL();
}

bool Application::InitSDL()
{
	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		m_Error = fmt::format("Failed to initialise SDL: {}", SDL_GetError());
		PAPI_ERROR("{}", m_Error);
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "SDL Initialisation Error", m_Error.c_str(), nullptr);
		return false;
	}
	
	return true;
}

void Application::ShutdownSDL()
{
	PAPI_TRACE("Shutting down SDL");
	SDL_Quit();
}
