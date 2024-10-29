#pragma once

class Renderer;
class Window;

struct ApplicationSpecification
{
	std::string AppName = "App";
	SemVer      Version = SemVer(1, 0, 0);
};

class Application
{
public:
	Application(ApplicationSpecification spec);
	~Application();

	bool Init();
	void Run();
	void Shutdown();

	void ShowError(const char *message, const char *title = "Error");

	NODISCARD static FORCEINLINE Application* Get() { return s_Instance; }

	NODISCARD FORCEINLINE bool             IsRunning() const { return m_Running; }
	NODISCARD FORCEINLINE bool             IsInitialised() const { return m_Initialised; }
	NODISCARD FORCEINLINE std::string_view GetError() const { return m_Error; }

protected:
	bool InitSDL();
	bool InitRenderer();

	void PreUpdate();
	void PollEvents();
	void Update();
	void Render();

	static void ShutdownSDL();

	static Application *s_Instance;

	ApplicationSpecification m_Specification;
	Ref<Window>              m_MainWindow;
	Ref<Renderer>            m_Renderer;
	bool                     m_Running     = false;
	bool                     m_Initialised = false;
	std::string              m_Error;
};
