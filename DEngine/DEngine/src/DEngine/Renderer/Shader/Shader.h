#pragma once

#include "string"
#include "DEngine/Core.h"

namespace DEngine
{
	class D_API Shader
	{
	public:
		virtual ~Shader() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		static Shader* Create(const std::string& vertexSrc, const std::string& fragSrc);
	protected:
		Shader() {}
	};
}
