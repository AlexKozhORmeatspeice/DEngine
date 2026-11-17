#include "dpch.h"
#include "OpenGLDescriptorSet.h"

#include "DEngine/Renderer/Buffers/UniformBuffer.h"
#include "DEngine/Renderer/Texture/Texture.h"

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace DEngine
{
	OpenGLDescriptorSet::OpenGLDescriptorSet(const Ref<PipelineLayout>& layout, uint32_t setIndex)
		: m_SetIndex(setIndex)
	{
	}

	void OpenGLDescriptorSet::WriteUniform(uint32_t binding, const Ref<UniformBuffer>& buffer)
	{
		m_Uniforms[binding] = buffer;
	}

	void OpenGLDescriptorSet::WriteTexture(uint32_t binding, const Ref<Texture>& texture)
	{
		m_Textures[binding] = texture;
	}

	void OpenGLDescriptorSet::Bind(CommandBuffer& commandBuffer, const Ref<PipelineLayout>& layout)
	{
		GLint program = 0;
		glGetIntegerv(GL_CURRENT_PROGRAM, &program);
		if (program == 0)
			return;

		if (m_SetIndex == 0)
		{
			auto camera = m_Uniforms.find(0);
			if (camera != m_Uniforms.end() && camera->second && camera->second->GetSize() >= sizeof(glm::mat4))
			{
				GLint location = glGetUniformLocation(program, "u_ViewProj");
				if (location >= 0)
					glUniformMatrix4fv(location, 1, GL_FALSE, static_cast<const float*>(camera->second->GetData()));
			}

			auto light = m_Uniforms.find(1);
			if (light != m_Uniforms.end() && light->second && light->second->GetSize() >= sizeof(glm::vec4) * 3)
			{
				const auto* values = static_cast<const glm::vec4*>(light->second->GetData());
				GLint direction = glGetUniformLocation(program, "u_LightDirection");
				GLint color = glGetUniformLocation(program, "u_LightColor");
				GLint viewPos = glGetUniformLocation(program, "u_ViewPos");
				if (direction >= 0)
					glUniform3fv(direction, 1, glm::value_ptr(values[0]));
				if (color >= 0)
					glUniform3fv(color, 1, glm::value_ptr(values[1]));
				if (viewPos >= 0)
					glUniform3fv(viewPos, 1, glm::value_ptr(values[2]));
			}
		}

		auto texture = m_Textures.find(0);
		if (texture != m_Textures.end() && texture->second)
		{
			texture->second->Bind(commandBuffer, 0);
			GLint location = glGetUniformLocation(program, "u_Texture");
			if (location >= 0)
				glUniform1i(location, 0);
		}
	}
}
