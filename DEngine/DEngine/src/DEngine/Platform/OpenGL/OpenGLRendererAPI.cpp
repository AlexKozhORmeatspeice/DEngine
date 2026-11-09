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
	void OpenGLRendererAPI::DrawIndexedWireframe(const DEngine::Ref<VertexArray> vertArr)
	{
		vertArr->Bind();

		// Triangle indexes are not line pairs. Draw the mesh edges, on top of the solid object.
		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glEnable(GL_POLYGON_OFFSET_LINE);
		glPolygonOffset(-1.0f, -1.0f);
		GLint depthFunc = GL_LESS;
		glGetIntegerv(GL_DEPTH_FUNC, &depthFunc);
		glDepthFunc(GL_LEQUAL);

		glDrawElements(GL_TRIANGLES, vertArr->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);

		glDepthFunc(depthFunc);
		glDisable(GL_POLYGON_OFFSET_LINE);
		glPolygonOffset(0.0f, 0.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_CULL_FACE);
	}
}