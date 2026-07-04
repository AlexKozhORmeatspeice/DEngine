#pragma once

#include "DEngine/Core.h"

#include "RenderCommand.h"
#include "DEngine/Renderer/Cameras/Camera.h"
#include "DEngine/Renderer/Shader/Shader.h"

namespace DEngine
{
	//TODO: сделать нормальную инициализацию
	class D_API Renderer
	{
	public:
		static void Init();
		static void BeginScene(const DEngine::Ref<Camera>& camera);
		static void EndScene();

		static void OnWindowResize(uint32_t width, uint32_t height);

		static void Submit(const DEngine::Ref<VertexArray>& vertexArray,
						   const DEngine::Ref<Shader>& shader,
						   const glm::mat4 trans = glm::mat4(1.0f));

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	private:
		struct SceneData
		{
			glm::mat4 viewProjMat;
		};

		static SceneData* s_SceneData;
	};

}
