#pragma once

#include "DEngine/Renderer/Pipeline/RendererPipeline.h"

namespace DEngine
{
	class D_API OpenGLRendererPipeline : public RendererPipeline
	{
	public:
		OpenGLRendererPipeline();
		explicit OpenGLRendererPipeline(Ref<Shader> shader, RenderDevice& device, const PipelineState& state);
		~OpenGLRendererPipeline() override = default;

		virtual void Bind(CommandBuffer& commandBuffer) override;
		virtual void SetViewport(CommandBuffer& commandBuffer, float x, float y, float width, float height) override;
	};
}
