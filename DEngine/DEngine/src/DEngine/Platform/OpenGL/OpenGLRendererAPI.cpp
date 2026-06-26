#include "dpch.h"
#include "OpenGLRendererAPI.h"

#include "glad/glad.h"

namespace DEngine
{
	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::DrawIndexed(const std::shared_ptr<VertexArray> vertArr)
	{
		vertArr->Bind();
		glDrawElements(GL_TRIANGLES, vertArr->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
	}
}