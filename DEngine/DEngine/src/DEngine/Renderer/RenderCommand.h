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
			if (s_RendererAPI)
				s_RendererAPI->Init();
		}

		inline static void Shutdown()
		{
			if (s_RendererAPI)
				s_RendererAPI->Shutdown();
		}

		inline static void SetClearColor(const glm::vec4& color)
		{
			if (s_RendererAPI)
				s_RendererAPI->SetClearColor(color);
		}

		inline static void SetViewport(uint32_t width, uint32_t height)
		{
			if (s_RendererAPI)
				s_RendererAPI->SetViewport(0, 0, width, height);
		}

		inline static void Clear()
		{
			if (s_RendererAPI)
				s_RendererAPI->Clear();
		}

		inline static void DrawIndexed(CommandBuffer& commandBuffer, const DEngine::Ref<VertexArray>& vertexArray)
		{
			if (s_RendererAPI && vertexArray)
				s_RendererAPI->DrawIndexed(commandBuffer, vertexArray);
		}
		inline static void DrawIndexedWireframe(CommandBuffer& commandBuffer, const DEngine::Ref<VertexArray>& vertexArray)
		{
			if (s_RendererAPI && vertexArray)
				s_RendererAPI->DrawIndexedWireframe(commandBuffer, vertexArray);
		}

	public:
		static RendererAPI* s_RendererAPI;
	};
}
