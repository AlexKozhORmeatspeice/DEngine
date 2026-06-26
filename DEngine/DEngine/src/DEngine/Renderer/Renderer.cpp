#include "dpch.h"
#include "Renderer.h"

namespace DEngine
{
	Renderer::SceneData* Renderer::s_SceneData = new Renderer::SceneData;

	void Renderer::BeginScene(const std::shared_ptr<Camera>& camera)
	{
		s_SceneData->viewProjMat = camera->GetViewProjMat();
	}

	void Renderer::EndScene()
	{
		
	}

	void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray,
						  const std::shared_ptr<Shader>& shader)
	{
		shader->Bind();
		shader->UploadUniformMat4("u_ViewProj", s_SceneData->viewProjMat);

		vertexArray->Bind();

		RenderCommand::DrawIndexed(vertexArray);
	}
}