﻿#pragma once

class Renderer;
class Window;

struct ApplicationSpecification
{
	std::string AppName = "App";
	SemVer      Version = SemVer(1, 0, 0);
};

class FPSCounter
{
public:
	FPSCounter();

	void                           AddSample(uint16_t fps);
	NODISCARD FORCEINLINE uint16_t GetFPS()
	{
		if (m_Dirty) RecalculateFPS();
		return m_FPS;
	};
	NODISCARD FORCEINLINE double GetAverageFrameTimeMS() { return 1000.0 / GetFPS(); }

private:
	void RecalculateFPS();

	constexpr static uint16_t s_MaxSamples = 200;

	uint16_t m_Samples[s_MaxSamples];
	uint8_t  m_sampleIndex = 0;
	uint16_t m_FPS         = 0;
	bool     m_Dirty       = true;
};

class Application
{
public:
	Application(const ApplicationSpecification &spec);
	Application(ApplicationSpecification &&spec);
	~Application();

	void Construct();
	bool Init();
	void BindDelegates();
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
	FPSCounter               m_FPSCounter;
	bool                     m_Running     = false;
	bool                     m_Initialised = false;
	std::string              m_Error;
};
