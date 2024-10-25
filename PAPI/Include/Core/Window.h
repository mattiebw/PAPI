#pragma once
#include <SDL3/SDL_video.h>

struct WindowSpecification
{
	std::string Title = "Window";
	
	// (0, 0) for default position. Ignored if Centered is true.
	glm::ivec2 Position = { 0, 0 };
	glm::ivec2 Size = { 800, 600 }, MinSize = { 0, 0 }, MaxSize = { 0, 0 };
	bool Centered = true;
	
	bool Resizable = true;
};

class Window
{
public:
	Window(const WindowSpecification &spec);

	void Show();
	void Hide();
	
	void Close();

protected:
	SDL_Window* m_Window = nullptr;
	WindowSpecification m_Specification;
};
