#include "papipch.h"
#include "Core/Window.h"

#include <SDL3/SDL_messagebox.h>
#include <SDL3/SDL_video.h>

Window::Window(const WindowSpecification &spec)
{
	m_Specification = spec;

	PAPI_TRACE("Creating window \"{}\"", spec.Title);

	m_Window = SDL_CreateWindow(spec.Title.c_str(), spec.Size.x, spec.Size.y, SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL);
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

	// Associate the Window object with the SDL window, so we can call Window functions from SDL events
	SDL_PropertiesID props = SDL_GetWindowProperties(m_Window);
	SDL_SetPointerProperty(props, "Window", this);

	// MW @gotcha: Don't set up delegates here because the window will likely move around in memory and the this pointer will be invalid
	
	Show();
}

Window::~Window()
{
	if (IsValid())
		Destroy();
}

SDL_GLContext Window::GetContext()
{
	if (m_Context == nullptr)
		m_Context = SDL_GL_CreateContext(m_Window);
	return m_Context;
}

void Window::Show()
{
	PAPI_ASSERT(m_Window && "Window is not initialised");
	SDL_ShowWindow(m_Window);
	m_IsVisible = true;
}

void Window::Hide()
{
	PAPI_ASSERT(m_Window && "Window is not initialised");
	SDL_HideWindow(m_Window);
	m_IsVisible = false;
}

void Window::Close()
{
	PAPI_TRACE("Closing window \"{}\"", m_Specification.Title);

	bool shouldDestroy = OnCloseRequested.Execute(this);
	if (!shouldDestroy)
		return;

	OnClose.Execute(this);

	if (m_Window != nullptr)
		SDL_DestroyWindow(m_Window);
	m_Window = nullptr;
}

void Window::Destroy()
{
	Close();
}

void Window::SetTitle(std::string_view title)
{
	SDL_SetWindowTitle(m_Window, title.data());
	m_Specification.Title = title;
}
