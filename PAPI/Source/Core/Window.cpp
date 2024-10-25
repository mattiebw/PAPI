#include "papipch.h"
#include "Core/Window.h"

#include <SDL3/SDL_messagebox.h>

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
