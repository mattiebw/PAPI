#include "papipch.h"
#include "Render/Renderer.h"

#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl3.h>

#include "Core/Application.h"
#include "Core/Window.h"
#include "Core/Input/Input.h"
#include "Render/Camera.h"
#include "Render/RenderBuffer.h"
#include "Render/ShaderLibrary.h"
#include "Render/Texture.h"
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

QuadBatch::QuadBatch(RendererData *data, uint32_t MaxQuads)
{
	m_Data        = data;
	m_MaxQuads    = MaxQuads;
	m_MaxVertices = MaxQuads * 4;
	m_MaxIndices  = MaxQuads * 6;

	m_QuadPositions[0] = {-0.5f, -0.5f, 0.0f, 1.0f};
	m_QuadPositions[1] = {0.5f, -0.5f, 0.0f, 1.0f};
	m_QuadPositions[2] = {0.5f, 0.5f, 0.0f, 1.0f};
	m_QuadPositions[3] = {-0.5f, 0.5f, 0.0f, 1.0f};

	m_QuadPositions3[0] = {-0.5f, -0.5f, 0.0f};
	m_QuadPositions3[1] = {0.5f, -0.5f, 0.0f};
	m_QuadPositions3[2] = {0.5f, 0.5f, 0.0f};
	m_QuadPositions3[3] = {-0.5f, 0.5f, 0.0f};

	m_VertexArray = CreateRef<VertexArray>();

	m_VertexBuffer = CreateRef<VertexBuffer>(static_cast<uint32_t>(m_MaxVertices * sizeof(QuadVertex)),
	                                         BufferUsageType::DynamicDraw);
	m_VertexBuffer->SetLayout(QuadVertex::GetLayout());
	m_VertexArray->AddVertexBuffer(m_VertexBuffer);

	uint32_t *quadIndices = new uint32_t[m_MaxIndices];
	for (uint32_t i = 0; i < m_MaxQuads; i++)
	{
		quadIndices[i * 6 + 0] = i * 4 + 0;
		quadIndices[i * 6 + 1] = i * 4 + 1;
		quadIndices[i * 6 + 2] = i * 4 + 2;
		quadIndices[i * 6 + 3] = i * 4 + 2;
		quadIndices[i * 6 + 4] = i * 4 + 3;
		quadIndices[i * 6 + 5] = i * 4 + 0;
	}
	Ref<IndexBuffer> indexBuffer = CreateRef<IndexBuffer>(quadIndices, m_MaxIndices);
	m_VertexArray->SetIndexBuffer(indexBuffer);
	delete[] quadIndices;

	m_TextureSlots.resize(m_Data->MaxTextureSlots, nullptr);

	m_Shader = ShaderLibrary::CreateShader("Quad");
	m_Shader->AddStageFromFile(GL_VERTEX_SHADER, "Content/Shaders/Quad.vert");
	m_Shader->AddStageFromFile(GL_FRAGMENT_SHADER, "Content/Shaders/Quad.frag");
	m_Shader->LinkProgram();

	m_VertexBufferBase = new QuadVertex[m_MaxVertices];
	m_VertexBufferPtr  = m_VertexBufferBase;
}

QuadBatch::~QuadBatch()
{
	delete[] m_VertexBufferBase;
}


// ----------------
// MW @todo @perf: Most quad drawing operations do not need a mat4 transform.
// Most of the time, we're just drawing a quad at a position with a size.
// This can be done with just a few vec multiplies, not multiple mat4 multiplies.
// In debug mode, over 80% of the time is spent in the mat4 multiplies.
// In release mode, it's still most of the frame - but we go up from ~15fps to ~400fps.
// ----------------

void QuadBatch::DrawQuad(const glm::mat4 &transform, const glm::vec4 &     tintColor, const glm::vec2 &texCoordMin,
                         const glm::vec2 &texCoordMax, const Ref<Texture> &texture)
{
	if (m_IndicesCount >= m_MaxIndices)
		Flush();

	// Find texture
	int32_t textureIndex = FindTexture(texture);
	
	for (size_t i = 0; i < 4; i++)
	{
		m_VertexBufferPtr->Position = transform * m_QuadPositions[i];
		m_VertexBufferPtr->Color    = tintColor;
		m_VertexBufferPtr->TexCoord = {
			i == 0 || i == 3 ? texCoordMin.x : texCoordMax.x, i == 0 || i == 1 ? texCoordMin.y : texCoordMax.y
		};
		m_VertexBufferPtr->TexIndex = static_cast<float>(textureIndex);
		m_VertexBufferPtr++;
	}

	m_IndicesCount += 6;
	m_Data->Stats.QuadCount++;
}

void QuadBatch::DrawQuad(const glm::mat4 &transform, const glm::vec4 &tintColor)
{
	DrawQuad(transform, tintColor, {0.0f, 0.0f}, {1.0f, 1.0f}, m_Data->WhiteTexture);
}

void QuadBatch::DrawQuad(const glm::vec3 &centerPosition, const glm::vec2 &size, const glm::vec2 &        texCoordMin,
                         const glm::vec2 &texCoordMax, const glm::vec4 &   tintColor, const Ref<Texture> &texture)
{
	if (m_IndicesCount >= m_MaxIndices)
		Flush();

	int32_t textureIndex = FindTexture(texture);

	glm::vec3 size3 = glm::vec3(size, 1.0f);
	for (size_t i = 0; i < 4; i++)
	{
		m_VertexBufferPtr->Position = centerPosition + m_QuadPositions3[i] * size3;
		m_VertexBufferPtr->Color    = tintColor;
		m_VertexBufferPtr->TexCoord = {
			i == 0 || i == 3 ? texCoordMin.x : texCoordMax.x, i == 0 || i == 1 ? texCoordMin.y : texCoordMax.y
		};
		m_VertexBufferPtr->TexIndex = static_cast<float>(textureIndex);
		m_VertexBufferPtr++;
	}

	m_IndicesCount += 6;
	m_Data->Stats.QuadCount++;
}

void QuadBatch::DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &tintColor)
{
	DrawQuad(position, size, {0.0f, 0.0f}, {1.0f, 1.0f}, tintColor, m_Data->WhiteTexture);
}

void QuadBatch::Flush()
{
	if (m_IndicesCount == 0)
		return; // Nothing to draw.

	// First, lets update our vertex buffer with our new data.
	m_VertexBuffer->SetData(m_VertexBufferBase,
	                        static_cast<uint32_t>(reinterpret_cast<uint8_t*>(m_VertexBufferPtr) - reinterpret_cast<
		                        uint8_t*>(m_VertexBufferBase)));

	// Bind our shader and its uniforms.
	m_Shader->Bind();
	if (Viewport *viewport = Renderer::GetCurrentViewport())
	{
		m_Shader->SetUniformMatrix4f("u_ViewProjection", viewport->GetCamera()->GetViewProjectionMatrix());
	}
	else
	{
		m_Shader->SetUniformMatrix4f("u_ViewProjection", glm::mat4(1.0f));
	}

	// Now let's bind our array and textures.
	for (int i = 0; i < m_TextureSlotIndex; i++)
		m_TextureSlots[i]->Activate(i);
	m_VertexArray->Bind();

	// Now we can render our quads.
	glDrawElements(GL_TRIANGLES, static_cast<int>(m_IndicesCount), GL_UNSIGNED_INT, nullptr);
	m_Data->Stats.DrawCalls++;

	// Now we can reset our state.
	Reset();
}

void QuadBatch::Reset()
{
	for (int i = 0; i < m_TextureSlotIndex; i++)
	{
		m_TextureSlots[i] = nullptr;
	}
	m_TextureSlotIndex = 0;
	m_VertexBufferPtr  = m_VertexBufferBase;
	m_IndicesCount     = 0;
}

int QuadBatch::FindTexture(const Ref<Texture> &texture)
{
	if (!texture) return -1;

	for (int i = 0; i < m_TextureSlotIndex; i++)
	{
		if (m_TextureSlots[i] == texture)
		{
			return i;
		}
	}

	if (m_TextureSlotIndex >= m_Data->MaxTextureSlots)
	{
		Flush();
		m_TextureSlotIndex = 0;
		m_TextureSlots[0]  = texture;

		return static_cast<int32_t>(m_TextureSlotIndex);
	}

	int index                          = static_cast<int32_t>(m_TextureSlotIndex);
	m_TextureSlots[m_TextureSlotIndex] = texture;
	m_TextureSlotIndex++;
	return index;
}

void RenderStats::Reset()
{
	memset(this, 0, sizeof(RenderStats));
}

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
	m_Data        = new RendererData();

	if (!InitOpenGL())
		return false;

	m_ImGUIInitialised = InitImGUI();

	m_Window->OnResize.BindMethod(this, &Renderer::OnWindowResize);

	m_QuadBatch = CreateRef<QuadBatch>(m_Data, 20000);

	return true;
}

bool Renderer::InitOpenGL()
{
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8); // MW @todo: Should this be part of the RendererSpec?

	#ifdef PAPI_GL_DEBUG
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
	#endif

	PAPI_ASSERT(m_Window && "Window should be set");
	m_Context = m_Window->GetGLContext();

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

	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &m_Data->MaxTextureSlots);
	m_Data->MaxTextureSlots = std::min(m_Data->MaxTextureSlots, 32); // Our shader system only supports 32 textures.

	// Setup error callback
	#ifdef PAPI_GL_DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(GLErrorCallback, nullptr);
	#endif

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LINE_SMOOTH);

	// Create our white texture
	uint32_t             whiteTextureData = 0xffffffff;
	TextureSpecification spec;
	spec.Width           = 1;
	spec.Height          = 1;
	spec.GenerateMipmaps = false;
	m_Data->WhiteTexture = CreateRef<Texture>(spec);
	m_Data->WhiteTexture->SetData(Buffer(reinterpret_cast<uint8_t*>(&whiteTextureData), sizeof(uint32_t)));

	SDL_GL_SetSwapInterval(m_Specification.VSync ? 1 : 0);
	PAPI_INFO("Initialised renderer");
	PAPI_INFO("   VSync: {}", m_Specification.VSync ? "On" : "Off");

	return true;
}

bool Renderer::InitImGUI()
{
	#ifndef PAPI_NO_IMGUI

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	ImGui::StyleColorsDark();
	if (!ImGui_ImplSDL3_InitForOpenGL(m_Window->GetHandle(), m_Window->GetGLContext()))
	{
		PAPI_ERROR("Failed to initialize ImGUI for SDL3/OpenGL.");
		return false;
	}

	if (!ImGui_ImplOpenGL3_Init("#version 130"))
	{
		PAPI_ERROR("Failed to initialize ImGUI for OpenGL.");
		return false;
	}

	// We'll add our nicer font here.
	// For now, we're just going to hard-code a content path.
	auto font      = io.Fonts->AddFontFromFileTTF("Content/Fonts/OpenSans-Regular.ttf", 16.0f);
	io.FontDefault = font;

	#endif

	return true;
}

bool Renderer::OnWindowResize(Window *window, const glm::ivec2 &size)
{
	// Here, we should layout our viewports. For now, we'll just have one viewport that takes up the whole window.
	if (m_Viewports.size() > 0)
		m_Viewports[0]->SetSize(size);
	return false;
}

void Renderer::ShutdownImGUI()
{
	#ifndef PAPI_NO_IMGUI

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();

	#endif
}

void Renderer::Shutdown()
{
	if (!m_Initialised)
		return;

	PAPI_TRACE("Shutting down renderer");

	ShutdownImGUI();

	// Clear all our shaders. Do so before we destroy the GL context, so we can clean up properly.
	ShaderLibrary::Clear();

	// Delete our batches.
	m_QuadBatch = nullptr;

	if (m_Window)
	{
		m_Window->DestroyGLContext();
		m_Window->OnResize.UnbindMethod(this, &Renderer::OnWindowResize);
		m_Window = nullptr;
	}

	m_Viewports.clear();

	delete m_Data;
	m_Data = nullptr;

	m_Initialised = false;
}

void Renderer::ProcessSDLEvent(const SDL_Event *e)
{
	#ifndef PAPI_NO_IMGUI
	ImGui_ImplSDL3_ProcessEvent(e);
	#endif
}

void Renderer::BeginFrame()
{
	m_Data->Stats.Reset();
}

void Renderer::Render()
{
	for (const auto &viewport : m_Viewports)
	{
		s_CurrentViewport = viewport.get();
		viewport->Render();
		m_QuadBatch->Flush();
	}
	s_CurrentViewport = nullptr;

	if (m_ImGUIInitialised)
		RenderImGUI();
}

void Renderer::RenderImGUI()
{
	#ifndef PAPI_NO_IMGUI
	ImGuiIO &io = ImGui::GetIO();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	DebugUIRenderCallback.Execute();

	if (Input::IsKeyDownThisFrame(PAPI_KEY_F3))
		m_DebugUIVisible = !m_DebugUIVisible;
	if (m_DebugUIVisible)
	{
		ImGui::Begin("Renderer Info", &m_DebugUIVisible);
		ImGui::Text("FPS: %d", Application::GetFPS());
		ImGui::Text("Draw Calls: %d", m_Data->Stats.DrawCalls);
		ImGui::Text("Quad Count: %d", m_Data->Stats.QuadCount);
		ImGui::End();
	}

	ImGui::Render();
	glViewport(0, 0, static_cast<int>(io.DisplaySize.x), static_cast<int>(io.DisplaySize.y));
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	#endif
}

void Renderer::EndFrame()
{
	SDL_GL_SwapWindow(m_Window->GetHandle());
}

Ref<Viewport> Renderer::CreateViewport()
{
	auto viewport = m_Viewports.emplace_back(CreateRef<Viewport>());
	viewport->SetSize(m_Window->GetSpec().Size);
	return viewport;
}

void Renderer::RemoveViewport(const Ref<Viewport> &viewport)
{
	auto it = std::find(m_Viewports.begin(), m_Viewports.end(), viewport);
	if (it != m_Viewports.end())
		m_Viewports.erase(it);
	else
		PAPI_WARN("Attempted to remove a viewport that has not been added!");
}

void Renderer::SetVSync(bool enabled)
{
	m_Specification.VSync = enabled;
	SDL_GL_SetSwapInterval(enabled ? 1 : 0);
}

void Renderer::GLErrorCallback(GLenum        source, GLenum       type, GLuint id, GLenum severity, GLsizei length,
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

	const char *sourceText;
	const char *typeText;
	const char *severityText;

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
