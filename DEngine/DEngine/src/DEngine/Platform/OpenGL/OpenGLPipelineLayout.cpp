#include "dpch.h"
#include "OpenGLPipelineLayout.h"

#include "glad/glad.h"
#include "glm/glm.hpp"

namespace DEngine
{
	OpenGLPipelineLayout::OpenGLPipelineLayout(RenderDevice& device, const PipelineLayoutState& state)
		: PipelineLayout(device, state)
	{
	}

	void OpenGLPipelineLayout::PushConstant(CommandBuffer& commandBuffer, uint32_t stageFlags, uint32_t offset, uint32_t size, const void* data)
	{
		if (!data || size < sizeof(glm::mat4))
			return;

		GLint program = 0;
		glGetIntegerv(GL_CURRENT_PROGRAM, &program);
		if (program == 0)
			return;

		GLint location = glGetUniformLocation(program, "u_ModelMat");
		if (location >= 0)
			glUniformMatrix4fv(location, 1, GL_FALSE, static_cast<const float*>(data));
	}
}
