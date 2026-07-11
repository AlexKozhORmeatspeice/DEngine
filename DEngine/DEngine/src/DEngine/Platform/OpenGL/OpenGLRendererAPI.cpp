#include "dpch.h"
#include "OpenGLRendererAPI.h"

#include "glad/glad.h"

namespace DEngine
{
	void OpenGLRendererAPI::Init()
	{
		//Init blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		//Init depth test
        glEnable(GL_DEPTH_TEST); 
		glDepthFunc(GL_LESS);

		glFrontFace(GL_CCW);
		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::DrawIndexed(const DEngine::Ref<VertexArray> vertArr)
	{
		vertArr->Bind();
		glDrawElements(GL_TRIANGLES, vertArr->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
	}
}