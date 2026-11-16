#include "dpch.h"
#include "OpenGLRendererPipeline.h"

namespace DEngine
{
	OpenGLRendererPipeline::OpenGLRendererPipeline()
	{
	}

	OpenGLRendererPipeline::OpenGLRendererPipeline(Ref<Shader> shader, RenderDevice& device, const PipelineState& state)
		: RendererPipeline(device, state)
	{
	}
}
