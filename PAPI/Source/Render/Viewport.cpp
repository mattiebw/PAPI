#include "papipch.h"
#include "Render/Viewport.h"

void Viewport::Render()
{
	glViewport(m_Offset.x, m_Offset.y, m_Size.x, m_Size.y);
	if (ShouldClear)
	{
		glClearColor(ClearColor.r, ClearColor.g, ClearColor.b, ClearColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}
