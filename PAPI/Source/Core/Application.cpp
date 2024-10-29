#include "papipch.h"
#include "Core/Application.h"

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_messagebox.h>
#include <SDL3/SDL_timer.h>

#include "PAPI.h"
#include "Core/Input/Input.h"
#include "Core/Window.h"
#include "Render/Renderer.h"

Application *Application::s_Instance = nullptr;

Application::Application(ApplicationSpecification spec)
	: m_Specification(std::move(spec))
{
	PAPI_ASSERT(s_Instance == nullptr && "There can only be one application instance");
	s_Instance = this;
}

Application::~Application()
{
	Shutdown();
	s_Instance = nullptr;
}

bool Application::Init()
{
	// First, initialise our core APIs.
	if (!InitSDL())
		return false;

	m_Initialised = true; // Set initialised to true so we can shut down properly.

	// Create the main window.
	m_MainWindow = CreateRef<Window>(WindowSpecification{
		.Title = m_Specification.AppName,
		.Centered = true
	});
	if (!m_MainWindow->IsValid()) // If the window is invalid, we can't continue.
	{
		m_Error = "Failed to create main window";
		Shutdown();
		return false;
	}

	if (!InitRenderer())
	{
		Shutdown();
		return false;
	}

	Input::Init();

	return true;
}

void Application::Run()
{
	m_Running = true;

	m_MainWindow->OnClose.BindLambda([](Window *window)
	{
		Get()->m_Running = false;
	});
	m_MainWindow->OnKeyPressed.BindLambda([](Window *window, Scancode scancode, bool repeat)
	{
		if (scancode == PAPI_KEY_ESCAPE)
			Get()->m_MainWindow->Close();
		else if (scancode == PAPI_KEY_R && !repeat)
		{
			g_ShouldRestart = !g_ShouldRestart;
			if (g_ShouldRestart)
				PAPI_INFO("PAPI will restart.");
			else
				PAPI_INFO("PAPI will close fully.");
		}

		return false;
	});

	uint64_t time = SDL_GetPerformanceCounter();
	uint64_t last = time;
	while (m_Running)
	{
		last = time;
		time = SDL_GetPerformanceCounter();
		const uint64_t delta = time - last;
		double deltaTime = static_cast<double>(delta * 1000) / static_cast<double>(SDL_GetPerformanceFrequency());

		PreUpdate();
		PollEvents();
		Update();
		Render();
	}
}

void Application::Shutdown()
{
	if (!m_Initialised)
		return;

	Input::Shutdown();
	if (m_Renderer)
	{
		m_Renderer->Shutdown();
		m_Renderer = nullptr;
	}
	if (m_MainWindow && m_MainWindow->IsValid())
	{
		m_MainWindow->Destroy();
		m_MainWindow = nullptr;
	}
	ShutdownSDL();
	m_Running     = false;
	m_Initialised = false;
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
		m_Error = "Failed to initialise renderer; check the log!";
		PAPI_ERROR("{}", m_Error);
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Renderer Initialisation Error", m_Error.c_str(), nullptr);
		return false;
	}

	return true;
}

void Application::PreUpdate()
{
	Input::PreUpdate();
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
				{
					// Skip creating a temporary glm::ivec2
					window->OnResize.Execute(std::move(window), *reinterpret_cast<const glm::ivec2*>(&e.window.data1));
				}
			}
			break;
		case SDL_EVENT_KEY_DOWN:
			{
				Input::ProcessKeyboardInputEvent(e.key);

				Window *    window    = m_MainWindow.get();
				SDL_Window *sdlWindow = SDL_GetWindowFromEvent(&e);
				if (sdlWindow)
				{
					SDL_PropertiesID props = SDL_GetWindowProperties(sdlWindow);
					window                 = static_cast<Window*>(SDL_GetPointerProperty(props, "Window", window));
				}
				PAPI_ASSERT(window && "No window, but key event?");
				window->OnKeyPressed.Execute(std::move(window), static_cast<Scancode>(e.key.scancode),
				                             std::move(e.key.repeat));
			}
			break;
		case SDL_EVENT_KEY_UP:
			{
				Input::ProcessKeyboardInputEvent(e.key);

				Window *    window    = m_MainWindow.get();
				SDL_Window *sdlWindow = SDL_GetWindowFromEvent(&e);
				if (sdlWindow)
				{
					SDL_PropertiesID props = SDL_GetWindowProperties(sdlWindow);
					window                 = static_cast<Window*>(SDL_GetPointerProperty(props, "Window", window));
				}
				PAPI_ASSERT(window && "No window, but key event?");
				window->OnKeyReleased.Execute(std::move(window), static_cast<Scancode>(e.key.scancode));
			}
			break;
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
		case SDL_EVENT_MOUSE_BUTTON_UP:
			{
				Input::ProcessMouseInputEvent(e.button);

				Window *    window    = m_MainWindow.get();
				SDL_Window *sdlWindow = SDL_GetWindowFromEvent(&e);
				if (sdlWindow)
				{
					SDL_PropertiesID props = SDL_GetWindowProperties(sdlWindow);
					window                 = static_cast<Window*>(SDL_GetPointerProperty(props, "Window", window));
				}
				PAPI_ASSERT(window && "No window, but mouse event?");

				if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
					window->OnMouseButtonDown.Execute(std::move(window), static_cast<MouseButton>(e.button.button));
				else
					window->OnMouseButtonUp.Execute(std::move(window), static_cast<MouseButton>(e.button.button));
			}
			break;
		case SDL_EVENT_MOUSE_MOTION:
			{
				Input::ProcessMouseMotionEvent(e.motion);

				Window *    window    = m_MainWindow.get();
				SDL_Window *sdlWindow = SDL_GetWindowFromEvent(&e);
				if (sdlWindow)
				{
					SDL_PropertiesID props = SDL_GetWindowProperties(sdlWindow);
					window                 = static_cast<Window*>(SDL_GetPointerProperty(props, "Window", window));
				}

				window->OnMouseMove.Execute(std::move(window), {e.motion.x, e.motion.y},
				                            {e.motion.xrel, e.motion.yrel});
			}
			break;
		}
	}
}

void Application::Update()
{
	if (Input::IsKeyDown(PAPI_KEY_Y))
		PAPI_INFO("Y is down!");

	if (Input::IsKeyDownThisFrame(PAPI_KEY_M))
	{
		PAPI_INFO("M is just down!");
		PAPI_INFO("Mouse pos: {}", Input::GetMousePosition());
	}

	// if (Input::IsKeyUp(PAPI_KEY_C))
	// 	PAPI_INFO("C is up!");

	if (Input::IsKeyUpThisFrame(PAPI_KEY_A))
		PAPI_INFO("A is just up!");

	if (Input::IsMouseButtonDownThisFrame(PAPI_MOUSE_BUTTON_LEFT))
		PAPI_INFO("LMB just down");

	if (Input::IsMouseButtonUpThisFrame(PAPI_MOUSE_BUTTON_MIDDLE))
		PAPI_INFO("MMB just up");

	glm::vec2 mouseDelta = Input::GetMouseDelta();
	if (mouseDelta != glm::vec2(0.0f))
		PAPI_INFO("Mouse delta: {}", mouseDelta);
}

void Application::Render()
{
	m_Renderer->BeginFrame();
	m_Renderer->Render();
	m_Renderer->EndFrame();
}

void Application::ShutdownSDL()
{
	PAPI_TRACE("Shutting down SDL");
	SDL_Quit();
}
