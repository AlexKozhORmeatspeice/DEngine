#pragma once

#include "DEngine/Core.h"
#include "RendererAPI.h"

#include "glm/glm.hpp"

namespace DEngine
{
	class D_API RenderCommand
	{
	public:
		inline static void Init()
		{
			s_RendererAPI->Init();
		}

		inline static void SetClearColor(const glm::vec4& color)
		{
			s_RendererAPI->SetClearColor(color);
		}

		inline static void SetViewport(uint32_t width, uint32_t height)
		{
			s_RendererAPI->SetViewport(0, 0, width, height);
		}

		inline static void Clear()
		{
			s_RendererAPI->Clear();
		}

		inline static void DrawIndexed(const DEngine::Ref<VertexArray>& vertexArray)
		{
			s_RendererAPI->DrawIndexed(vertexArray);
		}

	public:
		static RendererAPI* s_RendererAPI;
	};
}
