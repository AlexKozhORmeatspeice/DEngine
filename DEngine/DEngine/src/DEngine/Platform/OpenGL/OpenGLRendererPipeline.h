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
	};
}
