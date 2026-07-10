#pragma once

#include "DEngine/Core.h"
#include "DEngine/Asset/Asset.h"

#include "string"
#include "glm/glm.hpp"

#include "unordered_map"
#include "map"

namespace DEngine
{
	//-------Settings-------
	//Max number of shader program objects available to a GL program
	const int MAX_SHADER_SIZE = 4;
	//----------------------

	enum class ShaderType { None, Vertex, Fragment };

	using ShaderRegistry = std::map<ShaderType, std::string>;

	class D_API Shader : public Asset
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

		static Ref<Shader> Create(const ShaderRegistry& shaderRegistry, const std::string& name = "");

		static AssetType GetStaticType() { return AssetType::Shader; }
		virtual AssetType GetType() const override { return GetStaticType(); }
	protected:
		Shader() {}
	};
}
