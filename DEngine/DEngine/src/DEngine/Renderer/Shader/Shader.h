#pragma once

#include "DEngine/Core.h"

#include "string"
#include "glm/glm.hpp"

namespace DEngine
{
	class D_API Shader
	{
	public:
		virtual ~Shader() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void UploadUniformInt(const std::string& name, int val) = 0;
		virtual void UploadUniformFloat(const std::string& name, float val) = 0;
		virtual void UploadUniformFloat2(const std::string& name, const glm::vec2& val) = 0;
		virtual void UploadUniformFloat3(const std::string& name, const glm::vec3& val) = 0;
		virtual void UploadUniformFloat4(const std::string& name, const glm::vec4& val) = 0;
		virtual void UploadUniformMat3(const std::string& name, const glm::mat3& mat) = 0;
		virtual void UploadUniformMat4(const std::string& name, const glm::mat4& mat) = 0;

		static Shader* Create(const std::string& vertexSrc, const std::string& fragSrc);
	protected:
		Shader() {}
	};
}
