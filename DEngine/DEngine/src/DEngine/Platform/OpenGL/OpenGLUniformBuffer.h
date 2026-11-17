#pragma once

#include "DEngine/Renderer/Buffers/UniformBuffer.h"

#include "vector"

namespace DEngine
{
	class D_API OpenGLUniformBuffer : public UniformBuffer
	{
	public:
		explicit OpenGLUniformBuffer(uint32_t size);

		void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;
		const void* GetData() const override { return m_Data.data(); }
		uint32_t GetSize() const override { return static_cast<uint32_t>(m_Data.size()); }

	private:
		std::vector<uint8_t> m_Data;
	};
}
