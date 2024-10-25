#pragma once

struct ApplicationSpecification
{
	std::string AppName = "App";
	SemVer      Version = SemVer(1, 0, 0);
};

class Application
{
public:
	Application(const ApplicationSpecification& spec);

	bool Init();
	void Run();
	void Shutdown();

	[[nodiscard]] FORCEINLINE bool IsRunning() const { return m_Running; }
	[[nodiscard]] FORCEINLINE bool IsInitialised() const { return m_Initialised; }
	[[nodiscard]] FORCEINLINE std::string_view GetError() const { return m_Error; }

protected:
	bool InitSDL();

	void ShutdownSDL();
	
	ApplicationSpecification m_Specification;
	bool                     m_Running = false;
	bool                     m_Initialised = false;
	std::string              m_Error;
};
