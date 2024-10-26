#include "papipch.h"
#include "Core/Window.h"

#include <SDL3/SDL_messagebox.h>

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

	// Here are some random tests for the delegate system. To be removed.
	
	Echo.BindLambda([] (const std::string& str) { PAPI_INFO("Lambda. Echo: {}", str); });
	Echo.Execute("Lol");
	Echo.BindStatic(EchoTest);
	Echo.Execute("Lmao");
	Echo.Unbind();
	Echo.ExecuteIfBound("Unbound");
	
	IsWindowCool.BindLambda([] (const WindowSpecification& spec) { PAPI_INFO("Lambda. Title: {}", spec.Title); return spec.Title == "COOL"; });
	if (IsWindowCool.Execute({ .Title = "COOL" }))
		PAPI_INFO("Window is cool");
	else
		PAPI_INFO("Window is not cool..");
	IsWindowCool.BindMethod(this, &Window::WindowCoolTest);
	if (IsWindowCool.Execute({ .Title = "SWAGGER" }))
		PAPI_INFO("Window is cool");
	else
		PAPI_INFO("Window is not cool..");
	
	OnResize.BindStatic(&Test);
	OnResize.BindLambda([] (glm::vec2 vec) { PAPI_INFO("Lambda. X: {}, Y: {}", vec.x, vec.y); });
	OnResize.BindMethod(this, &Window::OnResizeTest);
	OnResize.Execute(glm::vec2(5, 5));
	OnResize.UnbindMethod(this, &Window::OnResizeTest);
	OnResize.Execute(glm::vec2(7, 7));

	CascadingTest.BindLambda([] (const std::string& str) { PAPI_INFO("Lambda. Str: {}", str); return str != "blarg"; });
	CascadingTest.BindLambda([this] (const std::string& str) { PAPI_INFO("Lambda. Str: {}", str); return str != "blarg" && m_Specification.Centered; });
	CascadingTest.BindMethod(this, &Window::CascadingTestFunc);
	CascadingTest.BindLambda([] (const std::string& str) { PAPI_INFO("Lambda. Str: {}", str); return str != "arg"; });
	CascadingTest.Execute("blegh");
	
	Show();
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

void Window::OnResizeTest(glm::vec2 newSize)
{
	PAPI_INFO("Method Test. X: {}, Y: {}", newSize.x, newSize.y);
}

bool Window::WindowCoolTest(const WindowSpecification &spec)
{
	PAPI_INFO("Method. Title: {}", spec.Title);
	return spec.Title == "SWAG";
}

bool Window::CascadingTestFunc(const std::string &str)
{
	PAPI_INFO("Method. Str: {}", str);
	return str != "blegh";
}
