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
		s_SceneData->cam = camera;
	}

	void Renderer::EndScene()
	{
		
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(width, height);
	}

	void Renderer::Submit(const Ref<Mesh>& mesh, const Ref<Material>& mat, const glm::mat4& trans, RenderMode mode)
	{
		mat->SetMat4("u_ViewProj", s_SceneData->viewProjMat);
		mat->SetMat4("u_ModelMat", trans);
		mat->SetFloat3("u_ViewPos", s_SceneData->cam->GetPos());

		mat->Bind();

		mesh->Bind();

		switch (mode)
		{
			case WIREFRAME:
				RenderCommand::DrawIndexedWireframe(mesh->GetVertexArray());
				break;
			case SOLID:
			default:
				RenderCommand::DrawIndexed(mesh->GetVertexArray());
				break;
		}

		mat->Unbind();
		mesh->Unbind();
	}
}