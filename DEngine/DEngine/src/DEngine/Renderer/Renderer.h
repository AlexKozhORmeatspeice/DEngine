#pragma once

#include "DEngine/Core.h"

#include "RenderCommand.h"
#include "DEngine/Renderer/Cameras/Camera.h"
#include "DEngine/Renderer/Shader/Shader.h"
#include "DEngine/Renderer/Mesh/Mesh.h"
#include "DEngine/Renderer/Material/Material.h"
#include "DEngine/Renderer/Pipeline/RendererPipeline.h"

#include "glm/glm.hpp"

enum RenderMode {
	SOLID, WIREFRAME
};

namespace DEngine
{
	class D_API Renderer
	{
	public:
		static void Init();
		static void BeginScene(const DEngine::Ref<Camera>& camera);
		static void EndScene();

		static void OnWindowResize(uint32_t width, uint32_t height);

		static void Submit(const Ref<Mesh>& mesh, const Ref<Material>& mat, const glm::mat4& trans, RenderMode mode = RenderMode::SOLID);

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	private:
		static Ref<RendererPipeline> m_Pipeline;

		struct SceneData
		{
			glm::mat4 viewProjMat;
			Ref<Camera> cam;
		};

		static SceneData* s_SceneData;
	};

}
