#pragma once
#include <SDL3/SDL_video.h>

class Viewport;
class Window;

class Renderer
{
public:
	Renderer() = default;
	~Renderer();

	bool Init(Ref<Window> window);
	void Shutdown();

	void BeginFrame();
	void Render();
	void EndFrame();

private:
	bool InitOpenGL();
	bool OnWindowResize(Window *window, const glm::ivec2 &size);

	bool          m_Initialised = false;
	Ref<Window>   m_Window      = nullptr;
	Ref<Viewport> m_Viewport    = nullptr; // For now, we only have one viewport. We could change this to 
	SDL_GLContext m_Context     = nullptr; // support multiple viewports, for example, for split-screen games.

	glm::vec4 m_ClearColor = {0.1f, 0.1f, 0.1f, 1};
};
