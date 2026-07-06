#pragma once

#include "DEngine/Core.h"

#include "string"
#include "glm/glm.hpp"

#include "unordered_map"

namespace DEngine
{
	class D_API Shader
	{
	public:
		virtual ~Shader() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void UploadUniformInt(const std::string& name, int val) = 0;
		virtual void UploadUniformInt(const uint32_t& u_loc, int val) = 0;
		virtual void UploadUniformFloat(const std::string& name, float val) = 0;
		virtual void UploadUniformFloat(const uint32_t& u_loc, float val) = 0;
		virtual void UploadUniformFloat2(const std::string& name, const glm::vec2& val) = 0;
		virtual void UploadUniformFloat2(const uint32_t& u_loc, const glm::vec2& val) = 0;
		virtual void UploadUniformFloat3(const std::string& name, const glm::vec3& val) = 0;
		virtual void UploadUniformFloat3(const uint32_t& u_loc, const glm::vec3& val) = 0;
		virtual void UploadUniformFloat4(const std::string& name, const glm::vec4& val) = 0;
		virtual void UploadUniformFloat4(const uint32_t& u_loc, const glm::vec4& val) = 0;
		virtual void UploadUniformMat3(const std::string& name, const glm::mat3& mat) = 0;
		virtual void UploadUniformMat3(const uint32_t& u_loc, const glm::mat3& mat) = 0;
		virtual void UploadUniformMat4(const std::string& name, const glm::mat4& mat) = 0;
		virtual void UploadUniformMat4(const uint32_t& u_loc, const glm::mat4& mat) = 0;

		virtual uint32_t GetUniformLocation(const std::string& name) = 0;

		virtual const std::string& GetName() const = 0;

		static Ref<Shader> Create(const std::string& filePath);
		static Ref<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragSrc);
	protected:
		Shader() {}
	};

	class D_API ShaderLibrary
	{
	public:
		void Add(const Ref<Shader>& shader);
		void Add(const std::string& name, const Ref<Shader>& shader);
		Ref<Shader> Load(const std::string& filePath);
		Ref<Shader> Load(const std::string& name, const std::string& filePath);

		Ref<Shader> Get(const std::string& name);
	private:
		std::unordered_map<std::string, Ref<Shader>> m_Shaders;
	};
}
