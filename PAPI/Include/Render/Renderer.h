#pragma once

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_video.h>

class Viewport;
class Window;

struct RendererSpecification
{
	bool VSync = false;
};

class Renderer
{
public:
	Renderer(RendererSpecification rendererSpecification);
	~Renderer();

	NODISCARD static FORCEINLINE Viewport* GetCurrentViewport() { return s_CurrentViewport; }

	bool Init(Ref<Window> window);
	void Shutdown();

	void ProcessSDLEvent(const SDL_Event *e);
	void BeginFrame();
	void Render();
	void EndFrame();

	void SetVSync(bool enabled);

	static void GLErrorCallback(GLenum        source,
	                            GLenum        type,
	                            GLuint        id,
	                            GLenum        severity,
	                            GLsizei       length,
	                            const GLchar *message,
	                            const void *  userParam);

private:
	bool InitOpenGL();
	bool InitImGUI();
	bool OnWindowResize(Window *window, const glm::ivec2 &size);

	void RenderImGUI();

	void ShutdownImGUI();

	RendererSpecification m_Specification;
	bool                  m_Initialised = false;
	bool                  m_ImGUIInitialised = false;
	Ref<Window>           m_Window = nullptr;
	Ref<Viewport>         m_Viewport = nullptr; // For now, we only have one viewport. We could change this to 
	SDL_GLContext         m_Context = nullptr; // support multiple viewports, for example, for split-screen games.

	static Viewport *s_CurrentViewport;
};
