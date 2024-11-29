#include "papipch.h"
#include "Core/Application.h"

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_messagebox.h>
#include <SDL3/SDL_timer.h>

#include "PAPI.h"
#include "Core/Layer.h"
#include "Core/Random.h"
#include "Core/Input/Input.h"
#include "Core/Window.h"
#include "Render/Font.h"
#include "World/World.h"
#include "Render/Renderer.h"

Application *Application::s_Instance = nullptr;

FPSCounter::FPSCounter()
{
	memset(&m_Samples, 0, sizeof(uint16_t) * s_MaxSamples);
}

void FPSCounter::AddSample(uint16_t fps)
{
	m_Samples[m_sampleIndex] = fps;
	m_sampleIndex            = (m_sampleIndex + 1) % s_MaxSamples;
	m_Dirty                  = true;
}

void FPSCounter::RecalculateFPS()
{
	int fpsTotal = 0;
	for (uint16_t m_Sample : m_Samples)
		fpsTotal += m_Sample;
	m_FPS   = static_cast<uint16_t>(fpsTotal / s_MaxSamples);
	m_Dirty = false;
}

Application::Application(const ApplicationSpecification &spec)
	: m_Specification(spec)
{
	Construct();
}

Application::Application(ApplicationSpecification &&spec)
	: m_Specification(std::move(spec))
{
	Construct();
}

Application::~Application()
{
	Shutdown();
	s_Instance = nullptr;
}

void Application::Construct()
{
	PAPI_ASSERT(s_Instance == nullptr && "There can only be one application instance");
	s_Instance = this;
}

bool Application::Init()
{
	// First, initialise our core APIs.
	if (!InitSDL())
		return false;

	m_Initialised = true; // Set initialised to true so we can shut down properly.

	m_SavedData.Init(std::filesystem::path(SDL_GetPrefPath("PAPI", "papi")) / "Saved");

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
	Random::Init();
	Font::InitFontSystem();

	return true;
}

void Application::BindDelegates()
{
	m_MainWindow->OnClose.BindLambda([](Window *window)
	{
		Get()->m_Running = false;
	});
	m_MainWindow->OnKeyPressed.BindLambda([](Window *window, Scancode scancode, bool repeat)
	{
		if (scancode == PAPI_KEY_F11)
			window->ToggleFullscreen();

		return false;
	});
}

void Application::Run()
{
	m_Running = true;

	BindDelegates();

	uint64_t time = SDL_GetPerformanceCounter();
	while (m_Running)
	{
		uint64_t last = time;
		time          = SDL_GetPerformanceCounter();
		m_DeltaTime   = (time - last) / static_cast<double>(SDL_GetPerformanceFrequency());

		m_FPSCounter.AddSample(static_cast<uint16_t>(1.0 / m_DeltaTime));

		static double timeSinceFPSPrinted = 0;
		if (timeSinceFPSPrinted >= 1)
		{
			PAPI_INFO("FPS: {} (frame time: {:.2f}ms)", m_FPSCounter.GetFPS(), m_FPSCounter.GetAverageFrameTimeMS());
			timeSinceFPSPrinted = 0;
		}
		else
		{
			timeSinceFPSPrinted += m_DeltaTime;
		}

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

	for (const Ref<World> &world : m_Worlds)
		world->Clean();
	m_Worlds.clear();

	Font::ShutdownFontSystem();
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

void Application::AddLayer(const Ref<Layer> &layer)
{
	m_Layers.push_back(layer);
	layer->OnAttach();
}

void Application::RemoveLayer(const Ref<Layer> &layer)
{
	layer->OnDetach();
	auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
	if (it != m_Layers.end())
		m_Layers.erase(it);
}

Ref<World> Application::AddWorld()
{
	return m_Worlds.emplace_back(CreateRef<World>());
}

void Application::RemoveWorld(const Ref<World> &world)
{
	auto it = std::find(m_Worlds.begin(), m_Worlds.end(), world);
	if (it != m_Worlds.end())
		m_Worlds.erase(it);
	else
		PAPI_ERROR("Attempted to remove a world that has not been added!");
}

Ref<World> Application::GetWorldFromPointer(const World *worldPtr)
{
	for (Ref<World> &world : m_Worlds)
		if (world.get() == worldPtr) return world;
	return nullptr;
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
	m_Renderer = CreateRef<Renderer>(RendererSpecification{
		.VSync = false
	});

	if (!m_Renderer->Init(m_MainWindow))
	{
		m_Error = "Failed to initialise renderer; check the log!";
		PAPI_ERROR("{}", m_Error);
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Renderer Initialisation Error", m_Error.c_str(), nullptr);
		return false;
	}

	m_Renderer->DebugUIRenderCallback.BindLambda([]()
	{
		for (const Ref<Layer> &layer : Get()->m_Layers)
		{
			layer->RenderImGUI(Get()->m_DeltaTime);
		}
	});

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
		m_Renderer->ProcessSDLEvent(&e);

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
		default: // Just here to suppress warnings.
			break;
		}
	}
}

void Application::Update()
{
	// Update our layers
	for (Ref<Layer> &layer : m_Layers)
		layer->Update(m_DeltaTime);

	// Tick all the worlds.
	for (Ref<World> &world : m_Worlds)
		world->Tick(m_DeltaTime);
}

void Application::Render()
{
	// Render our layers
	for (Ref<Layer> &layer : m_Layers)
		layer->Render(m_DeltaTime);

	if (!m_Renderer || !m_MainWindow || !m_MainWindow->IsValid())
		return;

	m_Renderer->BeginFrame();
	m_Renderer->Render();
	m_Renderer->EndFrame();
}

void Application::ShutdownSDL()
{
	PAPI_TRACE("Shutting down SDL");
	SDL_Quit();
}

void Layer::OnAttach()
{
}

void Layer::OnDetach()
{
}

void Layer::Update(double delta)
{
}

void Layer::Render(double delta)
{
}

void Layer::RenderImGUI(double delta)
{
}
