#include "dpch.h"
#include "Renderer.h"

namespace DEngine
{
	Renderer::SceneData* Renderer::s_SceneData = new Renderer::SceneData;

	void Renderer::Init()
	{
		RenderCommand::Init();
	}

	void Renderer::BeginScene(const DEngine::Ref<Camera>& camera)
	{
		s_SceneData->viewProjMat = camera->GetViewProjMat();
	}

	void Renderer::EndScene()
	{
		
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(width, height);
	}

	void Renderer::Submit(const DEngine::Ref<VertexArray>& vertexArray,
						  const DEngine::Ref<Shader>& shader,
						  const glm::mat4 trans)
	{
		shader->Bind();
		shader->UploadUniformMat4("u_ViewProj", s_SceneData->viewProjMat);
		shader->UploadUniformMat4("u_ModelMat", trans);

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
}