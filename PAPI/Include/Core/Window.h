#pragma once
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_video.h>

struct WindowSpecification
{
	std::string Title = "Window";

	// (0, 0) for default position. Ignored if Centered is true.
	glm::ivec2 Position = {0, 0};
	glm::ivec2 Size     = {800, 600}, MinSize = {0, 0}, MaxSize = {0, 0};
	bool       Centered = true;

	bool Resizable = true;
};

class Window
{
public:
	Window(const WindowSpecification &spec);
	~Window();

	NODISCARD SDL_GLContext           GetContext();
	NODISCARD FORCEINLINE SDL_Window* GetHandle() const { return m_Window; }

	void Show();
	void Hide();

	void Close();
	void Destroy();

	NODISCARD FORCEINLINE bool IsVisible() const { return m_IsVisible; };
	NODISCARD FORCEINLINE bool IsValid() const { return m_Window != nullptr; }

	CascadingMulticastDelegate<false, const glm::ivec2&>  OnResize;
	CascadingMulticastDelegate<false>                     OnCloseRequested;
	MulticastDelegate<>                                   OnClose;
	CascadingMulticastDelegate<false, SDL_Scancode, bool> OnKeyPressed;
	CascadingMulticastDelegate<false, SDL_Scancode>       OnKeyReleased;
	CascadingMulticastDelegate<false, uint8_t>            OnMouseButtonDown;
	CascadingMulticastDelegate<false, uint8_t>            OnMouseButtonUp;
	CascadingMulticastDelegate<false, const glm::vec2&>   OnMouseMove;

protected:
	bool                m_IsVisible = false;
	SDL_Window *        m_Window    = nullptr;
	SDL_GLContext       m_Context   = nullptr;
	WindowSpecification m_Specification;
};
