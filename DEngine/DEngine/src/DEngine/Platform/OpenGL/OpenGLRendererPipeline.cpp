#include "dpch.h"
#include "OpenGLRendererPipeline.h"

#include "glad/glad.h"

namespace DEngine
{
	OpenGLRendererPipeline::OpenGLRendererPipeline()
	{
	}

	OpenGLRendererPipeline::OpenGLRendererPipeline(Ref<Shader> shader, RenderDevice& device, const PipelineState& state)
		: RendererPipeline(device, state)
	{
	}

	void OpenGLRendererPipeline::Bind(CommandBuffer& commandBuffer)
	{
	}

	void OpenGLRendererPipeline::SetViewport(CommandBuffer& commandBuffer, float x, float y, float width, float height)
	{
		glViewport(static_cast<GLint>(x), static_cast<GLint>(y), static_cast<GLsizei>(width), static_cast<GLsizei>(height));
	}
}
