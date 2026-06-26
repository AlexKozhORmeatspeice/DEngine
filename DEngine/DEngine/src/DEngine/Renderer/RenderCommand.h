#pragma once

#include "DEngine/Core.h"
#include "RendererAPI.h"

#include "glm/glm.hpp"

namespace DEngine
{
	//TODO: сделать нормальную инициализацию
	class D_API RenderCommand
	{
	public:
		inline static void SetClearColor(const glm::vec4& color)
		{
			s_RendererAPI->SetClearColor(color);
		}

		inline static void Clear()
		{
			s_RendererAPI->Clear();
		}

		inline static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray)
		{
			s_RendererAPI->DrawIndexed(vertexArray);
		}

	public:
		static RendererAPI* s_RendererAPI;
	};
}
