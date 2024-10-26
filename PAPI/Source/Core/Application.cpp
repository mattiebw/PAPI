#include "papipch.h"
#include "Core/Application.h"

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_messagebox.h>

#include "Core/Window.h"

Application* Application::s_Instance = nullptr;

Application::Application(const ApplicationSpecification& spec)
{
	PAPI_ASSERT(s_Instance == nullptr && "There can only be one application instance");
	s_Instance = this;
	m_Specification = spec;
}

bool Application::Init()
{
	// First, initialise our core APIs.
	if (!InitSDL())
		return false;

	// Create the main window.
	m_MainWindow = CreateRef<Window>(WindowSpecification{
		.Title = m_Specification.AppName,
		.Centered = true
	});
	if (!m_MainWindow->IsValid()) // If the window is invalid, we can't continue.
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
	if (m_MainWindow && m_MainWindow->IsValid())
		m_MainWindow->Destroy();
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
