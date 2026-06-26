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

		virtual void UploadUniformMat4(const std::string& name, const glm::mat4& mat) = 0;

		static Shader* Create(const std::string& vertexSrc, const std::string& fragSrc);
	protected:
		Shader() {}
	};
}
