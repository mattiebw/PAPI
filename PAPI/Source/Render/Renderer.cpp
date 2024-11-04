#include "papipch.h"
#include "Render/Renderer.h"

#include "Core/Application.h"
#include "Core/Window.h"
#include "Render/Viewport.h"

Viewport *Renderer::s_CurrentViewport = nullptr;

// This is to enable the high-performance GPU on systems with both integrated and dedicated GPUs.
// MW @todo: This only works on Windows, and only with NVIDIA and AMD GPUs!
#ifdef PAPI_PLATFORM_WINDOWS
extern "C" {
_declspec(dllexport) DWORD NvOptimusEnablement                  = 1;
_declspec(dllexport) int   AmdPowerXpressRequestHighPerformance = 1;
}
#endif

Renderer::Renderer(RendererSpecification rendererSpecification)
	: m_Specification(std::move(rendererSpecification))
{
}

Renderer::~Renderer()
{
	Shutdown();
}

bool Renderer::Init(Ref<Window> window)
{
	PAPI_TRACE("Initialising renderer");

	m_Window      = window;
	m_Initialised = true;

	if (!InitOpenGL())
		return false;

	m_Window->OnResize.BindMethod(this, &Renderer::OnWindowResize);

	m_Viewport = CreateRef<Viewport>();

	return true;
}

bool Renderer::InitOpenGL()
{
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	#ifdef PAPI_GL_DEBUG
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
	#endif

	PAPI_ASSERT(m_Window && "Window should be set");
	m_Context = m_Window->GetContext();

	static bool glInitialised = false;
	if (!glInitialised)
	{
		int version = gladLoadGL(SDL_GL_GetProcAddress);
		if (version == 0)
		{
			const char *error = "Failed to initialise OpenGL with GLAD";
			PAPI_ERROR("{}", error);
			Application::Get()->ShowError(error, "OpenGL Error");
			return false;
		}
		glInitialised = true;
		
		PAPI_INFO("Initialised OpenGL v{}.{}", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));
		PAPI_INFO("   OpenGL Vendor: {}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
		PAPI_INFO("   OpenGL Renderer: {}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
	}

	// Setup error callback
	#ifdef PAPI_GL_DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(GLErrorCallback, nullptr);
	#endif
	
	SDL_GL_SetSwapInterval(m_Specification.VSync ? 1 : 0);
	PAPI_INFO("Initialised renderer");
	PAPI_INFO("   VSync: {}", m_Specification.VSync ? "On" : "Off");

	return true;
}

bool Renderer::OnWindowResize(Window *window, const glm::ivec2 &size)
{
	m_Viewport->SetSize(size);
	return false;
}

void Renderer::Shutdown()
{
	if (!m_Initialised)
		return;

	PAPI_TRACE("Shutting down renderer");

	if (m_Window)
	{
		m_Window->OnResize.UnbindMethod(this, &Renderer::OnWindowResize);
		m_Window = nullptr;
	}

	m_Viewport = nullptr;

	m_Initialised = false;
}

void Renderer::BeginFrame()
{
}

void Renderer::Render()
{
	PAPI_ASSERT(m_Viewport && "No viewport set");
	s_CurrentViewport = m_Viewport.get();
	m_Viewport->Render();
	s_CurrentViewport = nullptr;
}

void Renderer::EndFrame()
{
	SDL_GL_SwapWindow(m_Window->GetHandle());
}

void Renderer::SetVSync(bool enabled)
{
	m_Specification.VSync = enabled;
	SDL_GL_SetSwapInterval(enabled ? 1 : 0);
}

void Renderer::GLErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
	const GLchar *message, const void *userParam)
{
#ifndef PAPI_SHOW_GL_NOTIFICATIONS
	if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
		return;
#endif

#ifndef PAPI_NO_IGNORED_GL_ERROR_IDS
	static std::vector<GLuint> ignoredIDs = {131185};

	if (std::find(ignoredIDs.begin(), ignoredIDs.end(), id) != ignoredIDs.end())
		return;
#endif

	const char* sourceText;
	const char* typeText;
	const char* severityText;

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:
		sourceText = "API";
		break;

	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		sourceText = "Window System";
		break;

	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		sourceText = "Shader Compiler";
		break;

	case GL_DEBUG_SOURCE_THIRD_PARTY:
		sourceText = "Third Party";
		break;

	case GL_DEBUG_SOURCE_APPLICATION:
		sourceText = "Application";
		break;

	case GL_DEBUG_SOURCE_OTHER:
		sourceText = "Other";
		break;

	default:
		sourceText = "Unknown";
		break;
	}

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:
		typeText = "Error";
		break;

	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		typeText = "Deprecated Behaviour";
		break;

	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		typeText = "Undefined Behaviour";
		break;

	case GL_DEBUG_TYPE_PORTABILITY:
		typeText = "Portability";
		break;

	case GL_DEBUG_TYPE_PERFORMANCE:
		typeText = "Performance";
		break;

	case GL_DEBUG_TYPE_OTHER:
		typeText = "Other";
		break;

	case GL_DEBUG_TYPE_MARKER:
		typeText = "Marker";
		break;

	default:
		typeText = "Unknown";
		break;
	}

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:
		severityText = "High";
		break;

	case GL_DEBUG_SEVERITY_MEDIUM:
		severityText = "Medium";
		break;

	case GL_DEBUG_SEVERITY_LOW:
		severityText = "Low";
		break;

	case GL_DEBUG_SEVERITY_NOTIFICATION:
		severityText = "Notification";
		break;

	default:
		severityText = "Unknown";
		break;
	}

	PAPI_ERROR("OpenGL Error ({0} severity, id: {4}): from {1}, {2}: {3}", severityText, sourceText, typeText, message,
			 id);
	PAPI_ASSERT(severity == GL_DEBUG_SEVERITY_NOTIFICATION);
}
