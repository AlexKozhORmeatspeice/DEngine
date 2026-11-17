#pragma once

#include "DEngine/Core.h"

namespace DEngine
{
	class D_API UniformBuffer
	{
	public:
		virtual ~UniformBuffer() = default;

		static Ref<UniformBuffer> Create(uint32_t size);

		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) = 0;
		virtual const void* GetData() const = 0;
		virtual uint32_t GetSize() const = 0;
	};
}
