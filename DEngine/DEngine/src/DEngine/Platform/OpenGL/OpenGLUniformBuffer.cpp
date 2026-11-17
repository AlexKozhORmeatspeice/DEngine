#include "dpch.h"
#include "OpenGLUniformBuffer.h"

#include "cstring"

namespace DEngine
{
	OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size)
		: m_Data(size)
	{
	}

	void OpenGLUniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
	{
		if (!data || offset + size > m_Data.size())
			return;

		std::memcpy(m_Data.data() + offset, data, size);
	}
}
