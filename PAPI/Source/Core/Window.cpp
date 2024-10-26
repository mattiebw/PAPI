#include "papipch.h"
#include "Core/Window.h"

#include <SDL3/SDL_messagebox.h>
#include <SDL3/SDL_timer.h>

static void Test(glm::vec2 vec)
{
	PAPI_INFO("Test. X: {}, Y: {}", vec.x, vec.y);
}

static void EchoTest(const std::string& str)
{
	PAPI_INFO("Static Echo: {}", str);
}

Window::Window(const WindowSpecification& spec)
{
	m_Specification = spec;
	
	m_Window = SDL_CreateWindow(spec.Title.c_str(), spec.Size.x, spec.Size.y, SDL_WINDOW_HIDDEN);
	if (!m_Window)
	{
		std::string error = fmt::format("Failed to create window: {}", SDL_GetError());
		PAPI_ERROR("{}", error);
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Window Creation Error", error.c_str(), nullptr);
		return;
	}

	if (spec.Centered)
		SDL_SetWindowPosition(m_Window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	else
		SDL_SetWindowPosition(m_Window, spec.Position.x, spec.Position.y);

	SDL_SetWindowMinimumSize(m_Window, spec.MinSize.x, spec.MinSize.y);
	SDL_SetWindowMaximumSize(m_Window, spec.MaxSize.x, spec.MaxSize.y);
	SDL_SetWindowResizable(m_Window, spec.Resizable);

	Show();
	
	uint64_t begin = SDL_GetPerformanceCounter();
	for (int i = 0; i < 100000; i++)
	{
		Delegate<bool, glm::vec2> testDelegate;
		testDelegate.BindLambda([] (glm::vec2 v) { return v.x == 1; });
		testDelegate.Execute({ 1, 2 });

		Delegate<glm::vec3, glm::vec2> test2;
		test2.BindMethod(this, &Window::Test);
		test2.Execute({ 1, 2 });
	}
	uint64_t end = SDL_GetPerformanceCounter();
	auto result = fmt::format("Delegates x100000: {:.10f}s", (end - begin) / (float)SDL_GetPerformanceFrequency());
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Results", result.c_str(), m_Window);
}

Window::~Window()
{
	if (IsValid())
		Destroy();
}

void Window::Show()
{
	PAPI_ASSERT(m_Window && "Window is not initialised");
	SDL_ShowWindow(m_Window);
}

void Window::Hide()
{
	PAPI_ASSERT(m_Window && "Window is not initialised");
	SDL_HideWindow(m_Window);
}

void Window::Destroy()
{
	PAPI_INFO("Destroying window '{}'", m_Specification.Title);
	if (m_Window != nullptr)
		SDL_DestroyWindow(m_Window);
	m_Window = nullptr;
}

glm::vec3 Window::Test(glm::vec2 vec)
{
	return glm::vec3(vec, 0.0f);
}
