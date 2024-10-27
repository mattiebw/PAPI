#pragma once

class Camera;
class Window;

// Viewports contain the camera, and their width and height and offset in the window.
// They also contain the UI elements that are drawn on top of the scene.

class Viewport
{
public:
	Viewport() = default;

	void Render();

	NODISCARD FORCEINLINE Ref<Camera> GetCamera() const { return m_Camera; }
	NODISCARD FORCEINLINE glm::ivec2  GetOffset() const { return m_Offset; }
	NODISCARD FORCEINLINE glm::ivec2  GetSize() const { return m_Size; }

	FORCEINLINE void SetCamera(Ref<Camera> camera) { m_Camera = camera; }
	FORCEINLINE void SetOffset(const glm::ivec2 &offset) { m_Offset = offset; }
	FORCEINLINE void SetSize(const glm::ivec2 &size) { m_Size = size; }
	FORCEINLINE void SetOffsetAndSize(const glm::ivec2 &offset, const glm::ivec2 &size)
	{
		m_Offset = offset;
		m_Size   = size;
	}

	bool      ShouldClear = true;
	glm::vec4 ClearColor  = {0.1f, 0.1f, 0.1f, 1};

protected:
	Ref<Camera> m_Camera;
	glm::ivec2  m_Offset;
	glm::ivec2  m_Size;
};
