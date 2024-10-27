#pragma once
#include <SDL3/SDL_video.h>

class Window;

class Renderer
{
public:
	Renderer() = default;
	~Renderer();

	bool Init(Ref<Window> window);
	void Shutdown();

private:
	bool InitOpenGL();

	bool          m_Initialised = false;
	Ref<Window>   m_Window      = nullptr;
	SDL_GLContext m_Context     = nullptr;
};
