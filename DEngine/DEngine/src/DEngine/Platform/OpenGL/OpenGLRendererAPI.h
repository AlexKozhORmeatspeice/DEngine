#pragma once
#include "DEngine/Core.h"

#include "DEngine/Renderer/RendererAPI.h"

namespace DEngine
{
	class D_API OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void Init() override;
		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;

		virtual void DrawIndexed(const DEngine::Ref<VertexArray> vertArr) override;
	};
}
