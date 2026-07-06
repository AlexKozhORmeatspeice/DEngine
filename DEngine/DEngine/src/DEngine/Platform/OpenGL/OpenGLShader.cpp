#include "dpch.h"
#include "OpenGLShader.h"

#include "fstream"

#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"

namespace DEngine
{
	//-------Settings-------
	//Max number of shader program object available to a GL program
	const int MAX_SHADER_SIZE = 4;
	//----------------------

	static GLenum ShaderTypeFromString(const std::string& str)
	{
		if (str == "vertex")
			return GL_VERTEX_SHADER;

		if (str == "fragment" || str == "pixel")
			return GL_FRAGMENT_SHADER;

		D_CORE_ASSERT(false, "Unkown shader type!");
		return 0;
	}

	OpenGLShader::OpenGLShader(const std::string& filePath)
	{
		std::string shaderSource = ReadFile(filePath);
		auto shaderSources = Preprocess(shaderSource);
		Compile(shaderSources);

		//Extract name from file path
		auto lastSlash = filePath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;

		auto lastDot = filePath.rfind(".");
		auto count = lastDot == std::string::npos ? filePath.size() - lastSlash : lastDot - lastSlash;

		m_Name = filePath.substr(lastSlash, count);
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragSrc)
		: m_Name(name)
	{
		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexSrc;
		sources[GL_FRAGMENT_SHADER] = fragSrc;

		Compile(sources);
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

	std::string OpenGLShader::ReadFile(const std::string& filePath)
	{
		std::string res;
		std::ifstream in(filePath, std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			res.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&res[0], res.size());
			in.close();
		}
		else
		{
			D_CORE_ERROR("Can't load shader by path {0}", filePath);
		}
		
		return res;
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::Preprocess(const std::string& source)
	{
		std::unordered_map<GLenum, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLenght = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			size_t eol = source.find("\r\n", pos);
			D_CORE_ASSERT(eol != std::string::npos, "Shader syntax error");
			
			size_t begin = pos + typeTokenLenght + 1;
			std::string type = source.substr(begin, eol - begin);
			D_CORE_ASSERT(ShaderTypeFromString(type), "Invalid shader type");

			size_t nextLinePos = source.find("\r\n", eol);
			pos = source.find(typeToken, nextLinePos);
			shaderSources[ShaderTypeFromString(type)] = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
		}

		return shaderSources;
	}

	void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		GLuint prog = glCreateProgram();

		D_CORE_ASSERT(shaderSources.size() <= MAX_SHADER_SIZE, "Too many shaders passed. Increase max number of shaders in engine API");

		std::array<GLuint, MAX_SHADER_SIZE> glShaderIds;

		int i = 0;
		for (auto&& [type, source] : shaderSources)
		{
			GLuint shader = glCreateShader(type);

			//Create shader handle
			const GLchar* gl_source = source.c_str();
			glShaderSource(shader, 1, &gl_source, 0);

			// Compile the vertex shader
			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if(isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				// The maxLength includes the NULL character
				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);
				
				// We don't need the shader anymore.
				glDeleteShader(shader);
				
				D_CORE_ERROR("{0}", infoLog.data());
				D_CORE_ASSERT(false, "Shader compilation error");
				break;
			}

			glAttachShader(prog, shader);
			glShaderIds[i++] = shader;
		}

		// Link our program
		glLinkProgram(prog);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(prog, GL_LINK_STATUS, (int *)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(prog, maxLength, &maxLength, &infoLog[0]);
			
			// We don't need the program anymore.
			glDeleteProgram(prog);

			for (const auto& glShader : glShaderIds)
			{
				glDeleteShader(glShader);
			}

			// Use the infoLog as you see fit.
			
			D_CORE_ERROR("{0}", infoLog.data());
			D_CORE_ASSERT(false, "Shader linking error");
			return;
		}

		// Always detach shaders after a successful link.
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