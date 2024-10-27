#include "papipch.h"
#include "Core/Application.h"

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_messagebox.h>
#include <SDL3/SDL_timer.h>

#include "PAPI.h"
#include "Core/Window.h"
#include "Render/Renderer.h"

Application *Application::s_Instance = nullptr;

Application::Application(const ApplicationSpecification &spec)
{
	PAPI_ASSERT(s_Instance == nullptr && "There can only be one application instance");
	s_Instance      = this;
	m_Specification = spec;
}

Application::~Application()
{
	s_Instance = nullptr;
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
	{
		Shutdown();
		return false;
	}

	if (!InitRenderer())
	{
		Shutdown();
		return false;
	}

	return true;
}

void Application::Run()
{
	m_Running = true;

	m_MainWindow->OnClose.BindLambda([this](Window* window) { m_Running = false; });
	m_MainWindow->OnKeyPressed.BindLambda([this](Window* window, SDL_Scancode scancode, bool repeat)
	{
		if (scancode == SDL_SCANCODE_ESCAPE)
			m_Running = false;
		else if (scancode == SDL_SCANCODE_R && !repeat)
		{
			g_ShouldRestart = !g_ShouldRestart;
			if (g_ShouldRestart)
				PAPI_INFO("PAPI will restart.");
			else
				PAPI_INFO("PAPI will close fully.");
		}

		return false;
	});
	m_MainWindow->OnResize.BindLambda([](Window* window, const glm::ivec2 &size) { PAPI_INFO("Window {} resized to {}", window->GetTitle(), size); return false; });

	uint64_t time = SDL_GetPerformanceCounter();
	uint64_t last = time;
	while (m_Running)
	{
		last = time;
		time = SDL_GetPerformanceCounter();
		const uint64_t delta = time - last;
		double deltaTime = static_cast<double>(delta * 1000) / static_cast<double>(SDL_GetPerformanceFrequency());

		PollEvents();
		Update();
		Render();
	}
}

void Application::Shutdown()
{
	if (m_Renderer)
		m_Renderer->Shutdown();
	if (m_MainWindow && m_MainWindow->IsValid())
		m_MainWindow->Destroy();
	ShutdownSDL();
}

void Application::ShowError(const char *message, const char *title)
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, message, nullptr);
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

bool Application::InitRenderer()
{
	m_Renderer = CreateRef<Renderer>();
	if (!m_Renderer->Init(m_MainWindow))
	{
		m_Error = "Failed to initialise renderer";
		PAPI_ERROR("{}", m_Error);
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Renderer Initialisation Error", m_Error.c_str(), nullptr);
		return false;
	}

	return true;
}

void Application::PollEvents()
{
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
		case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
			{
				SDL_PropertiesID props = SDL_GetWindowProperties(SDL_GetWindowFromEvent(&e));
				if (Window *window = static_cast<Window*>(SDL_GetPointerProperty(props, "Window", nullptr)))
					window->Close();
			}
			break;
		case SDL_EVENT_WINDOW_RESIZED:
			{
				SDL_PropertiesID props = SDL_GetWindowProperties(SDL_GetWindowFromEvent(&e));
				if (Window *window = static_cast<Window*>(SDL_GetPointerProperty(props, "Window", nullptr)))
					window->OnResize.Execute(std::move(window), *reinterpret_cast<const glm::ivec2*>(&e.window.data1));
				// Skip creating a temporary glm::ivec2
			}
			break;
		case SDL_EVENT_KEY_DOWN:
			{
				Window *    window    = m_MainWindow.get();
				SDL_Window *sdlWindow = SDL_GetWindowFromEvent(&e);
				if (sdlWindow)
				{
					SDL_PropertiesID props = SDL_GetWindowProperties(sdlWindow);
					window                 = static_cast<Window*>(SDL_GetPointerProperty(props, "Window", window));
				}
				PAPI_ASSERT(window && "No window, but key event?");
				window->OnKeyPressed.Execute(std::move(window), std::move(e.key.scancode), std::move(e.key.repeat));
			}
			break;
		case SDL_EVENT_KEY_UP:
			{
				Window *    window    = m_MainWindow.get();
				SDL_Window *sdlWindow = SDL_GetWindowFromEvent(&e);
				if (sdlWindow)
				{
					SDL_PropertiesID props = SDL_GetWindowProperties(sdlWindow);
					window                 = static_cast<Window*>(SDL_GetPointerProperty(props, "Window", window));
				}
				PAPI_ASSERT(window && "No window, but key event?");
				window->OnKeyReleased.Execute(std::move(window), std::move(e.key.scancode));
			}
			break;
		}
	}
}

void Application::Update()
{
}

void Application::Render()
{
	m_Renderer->BeginFrame();
	m_Renderer->EndFrame();
}

void Application::ShutdownSDL()
{
	PAPI_TRACE("Shutting down SDL");
	SDL_Quit();
}
