#include "dpch.h"
#include "OpenGLShader.h"

#include "fstream"

#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"

namespace DEngine
{
	static GLenum ShaderTypeToGLType(ShaderType type)
	{
		if (type == ShaderType::Vertex)
			return GL_VERTEX_SHADER;

		if (type == ShaderType::Fragment)
			return GL_FRAGMENT_SHADER;

		D_CORE_ASSERT(false, "Unkown shader type!");
		return GL_NONE;
	}

	OpenGLShader::OpenGLShader(const ShaderRegistry& shaderRegistry, const std::string& name)
		: m_Name(name)
	{
		Compile(shaderRegistry);
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_RendererID);
	}

	void OpenGLShader::Bind() const
	{
		glUseProgram(m_RendererID);
	}

	void OpenGLShader::Unbind() const
	{
		glUseProgram(0);
	}


	void OpenGLShader::Compile(const ShaderRegistry& shaderSources)
	{
		GLuint prog = glCreateProgram();

		D_CORE_ASSERT(shaderSources.size() <= MAX_SHADER_SIZE, "Too many shaders passed. Increase max number of shaders in engine API");

		std::array<GLuint, MAX_SHADER_SIZE> glShaderIds;

		int i = 0;
		for (auto&& [type, source] : shaderSources)
		{
			GLuint shader = glCreateShader(ShaderTypeToGLType(type));

			//Create shader handle
			const GLchar* gl_source = source.c_str();
			glShaderSource(shader, 1, &gl_source, 0);

			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if(isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);
				
				glDeleteShader(shader);
				
				D_CORE_ERROR("{0}", infoLog.data());
				D_CORE_ASSERT(false, "Shader compilation error");
				break;
			}

			glAttachShader(prog, shader);
			glShaderIds[i++] = shader;
		}

		glLinkProgram(prog);

		GLint isLinked = 0;
		glGetProgramiv(prog, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(prog, maxLength, &maxLength, &infoLog[0]);
			
			glDeleteProgram(prog);

			for (const auto& glShader : glShaderIds)
			{
				glDeleteShader(glShader);
			}

			D_CORE_ERROR("{0}", infoLog.data());
			D_CORE_ASSERT(false, "Shader linking error");
			return;
		}

		for (const auto& glShader : glShaderIds)
		{
			glDetachShader(prog, glShader);
		}

		m_RendererID = prog;
	}

	void OpenGLShader::UploadUniformInt(const std::string& name, int val)
	{
		GLint u_loc = glGetUniformLocation(m_RendererID, name.c_str());
		UploadUniformInt(u_loc, val);
	}

	void OpenGLShader::UploadUniformInt(const uint32_t& u_loc, int val)
	{
		glUniform1i(u_loc, val);
	}
	
	void OpenGLShader::UploadUniformFloat(const std::string& name, float val)
	{
		GLint u_loc = glGetUniformLocation(m_RendererID, name.c_str());
		UploadUniformFloat(u_loc, val);
	}

	void OpenGLShader::UploadUniformFloat(const uint32_t& u_loc, float val)
	{
		glUniform1f(u_loc, val);
	}

	void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& val)
	{
		GLint u_loc = glGetUniformLocation(m_RendererID, name.c_str());
		UploadUniformFloat2(u_loc, val);
	}

	void OpenGLShader::UploadUniformFloat2(const uint32_t& u_loc, const glm::vec2& val)
	{
		glUniform2f(u_loc, val.x, val.y);
	}

	void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& val)
	{
		GLint u_loc = glGetUniformLocation(m_RendererID, name.c_str());
		UploadUniformFloat3(u_loc, val);
	}

	void OpenGLShader::UploadUniformFloat3(const uint32_t& u_loc, const glm::vec3& val)
	{
		glUniform3f(u_loc, val.x, val.y, val.z);
	}

	void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& val)
	{
		GLint u_loc = glGetUniformLocation(m_RendererID, name.c_str());
		UploadUniformFloat4(u_loc, val);
	}

	void OpenGLShader::UploadUniformFloat4(const uint32_t& u_loc, const glm::vec4& val)
	{
		glUniform4f(u_loc, val.x, val.y, val.z, val.w);
	}

	void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& mat)
	{
		GLint u_loc = glGetUniformLocation(m_RendererID, name.c_str());
		UploadUniformMat3(u_loc, mat);
	}

	void OpenGLShader::UploadUniformMat3(const uint32_t& u_loc, const glm::mat3& mat)
	{
		glUniformMatrix3fv(u_loc, 1, GL_FALSE, glm::value_ptr(mat));
	}

	void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& mat)
	{
		GLint u_loc = glGetUniformLocation(m_RendererID, name.c_str());
		UploadUniformMat4(u_loc, mat);
	}

	void OpenGLShader::UploadUniformMat4(const uint32_t& u_loc, const glm::mat4& mat)
	{
		glUniformMatrix4fv(u_loc, 1, GL_FALSE, glm::value_ptr(mat));
	}

	uint32_t OpenGLShader::GetUniformLocation(const std::string& name)
	{
		GLint u_loc = glGetUniformLocation(m_RendererID, name.c_str());
		D_CORE_ASSERT(u_loc != -1, "Can't find value for material uniform");

		return u_loc;
	}
}