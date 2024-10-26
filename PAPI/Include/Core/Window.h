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
	~Window();

	void Show();
	void Hide();
	
	void Close();
	void Destroy();

	NODISCARD bool IsOpen() const;
	NODISCARD FORCEINLINE bool IsValid() const { return m_Window != nullptr; }

	void OnResizeTest(glm::vec2 newSize);
	bool WindowCoolTest(const WindowSpecification& spec);
	bool CascadingTestFunc(const std::string& str);

	Delegate<void, const std::string&> Echo;
	Delegate<bool, const WindowSpecification&> IsWindowCool;
	MulticastDelegate<glm::vec2> OnResize;
	CascadingMulticastDelegate<true, const std::string&> CascadingTest;
	
protected:
	SDL_Window* m_Window = nullptr;
	WindowSpecification m_Specification;
};
