#include "dpch.h"
#include "OpenGLShader.h"

#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"

namespace DEngine
{
	OpenGLShader::OpenGLShader(const std::string& vertexSrc, const std::string& fragSrc)
	{
		//Create shader handle
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		const GLchar* source = vertexSrc.c_str();
		glShaderSource(vertexShader, 1, &source, 0);

		// Compile the vertex shader
		glCompileShader(vertexShader);

		GLint isCompiled = 0;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
		if(isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);
			
			// We don't need the shader anymore.
			glDeleteShader(vertexShader);
			
			D_CORE_ERROR("{0}", infoLog.data());
			D_CORE_ASSERT(false, "Vertex Shader compilation error");
			return;
		}

		// Create an empty fragment shader handle
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		// Send the fragment shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		source = (const GLchar *)fragSrc.c_str();
		glShaderSource(fragmentShader, 1, &source, 0);

		// Compile the fragment shader
		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);
			
			// We don't need the shader anymore.
			glDeleteShader(fragmentShader);
			// Either of them. Don't leak shaders.
			glDeleteShader(vertexShader);

			// Use the infoLog as you see fit.

			D_CORE_ERROR("{0}", infoLog.data());
			D_CORE_ASSERT(false, "Fragment Shader compilation error");
			return;
		}

		// Vertex and fragment shaders are successfully compiled.
		// Now time to link them together into a program.
		// Get a program object.
		m_RendererID = glCreateProgram();

		// Attach our shaders to our program
		glAttachShader(m_RendererID, vertexShader);
		glAttachShader(m_RendererID, fragmentShader);

		// Link our program
		glLinkProgram(m_RendererID);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(m_RendererID, GL_LINK_STATUS, (int *)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(m_RendererID, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(m_RendererID, maxLength, &maxLength, &infoLog[0]);
			
			// We don't need the program anymore.
			glDeleteProgram(m_RendererID);
			// Don't leak shaders either.
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			// Use the infoLog as you see fit.
			
			D_CORE_ERROR("{0}", infoLog.data());
			D_CORE_ASSERT(false, "Shader linking error");
			return;
		}

		// Always detach shaders after a successful link.
		glDetachShader(m_RendererID, vertexShader);
		glDetachShader(m_RendererID, fragmentShader);
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

	void OpenGLShader::UploadUniformInt(const std::string& name, int val)
	{
		GLint u_loc = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1i(u_loc, val);
	}
	
	void OpenGLShader::UploadUniformFloat(const std::string& name, float val)
	{
		GLint u_loc = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1f(u_loc, val);
	}

	void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& val)
	{
		GLint u_loc = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform2f(u_loc, val.x, val.y);
	}

	void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& mat)
	{
		GLint u_loc = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform3f(u_loc, mat.x, mat.y, mat.z);
	}

	void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& val)
	{
		GLint u_loc = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4f(u_loc, val.x, val.y, val.z, val.w);
	}

	void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& mat)
	{
		GLint u_loc = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix3fv(u_loc, 1, GL_FALSE, glm::value_ptr(mat));
	}

	void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& mat)
	{
		GLint u_loc = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix4fv(u_loc, 1, GL_FALSE, glm::value_ptr(mat));
	}
}