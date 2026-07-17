#include "dpch.h"
#include "OpenGLBuffer.h"

#include "glad/glad.h"

namespace DEngine
{
//----------------VERTEX BUFFER----------------

	OpenGLVertexBuffer::OpenGLVertexBuffer(float* verts, uint32_t size)
		: VertexBuffer()
	{
		glCreateBuffers(1, &m_RenderID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RenderID);
        glBufferData(GL_ARRAY_BUFFER, size, verts, GL_STATIC_DRAW);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		glDeleteBuffers(1, &m_RenderID);
	}

	void OpenGLVertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RenderID);
	}

	void OpenGLVertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
//---------------------------------------------
// 
//----------------INDEX BUFFER----------------
	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* inds, uint32_t count)
		: m_Count(count)
	{
		glCreateBuffers(1, &m_RenderID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RenderID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), inds, GL_STATIC_DRAW);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &m_RenderID);
	}

	void OpenGLIndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RenderID);
	}

	void OpenGLIndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
//--------------------------------------------
}