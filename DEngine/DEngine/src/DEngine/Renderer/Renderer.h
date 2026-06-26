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
		static void BeginScene(const std::shared_ptr<Camera>& camera);
		static void EndScene();

		static void Submit(const std::shared_ptr<VertexArray>& vertexArray,
						   const std::shared_ptr<Shader>& shader);

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	private:
		struct SceneData
		{
			glm::mat4 viewProjMat;
		};

		static SceneData* s_SceneData;
	};

}
