#include "dpch.h"
#include "OpenGLVertexArray.h"

#include "glad/glad.h"

namespace DEngine
{
    static GLenum ShaderDataTypeToOpenGLType(ShaderDataType type)
    {
        switch (type)
        {
			case DEngine::ShaderDataType::None:   return GL_FLOAT;
			case DEngine::ShaderDataType::Float:  return GL_FLOAT;
			case DEngine::ShaderDataType::Float2: return GL_FLOAT;
			case DEngine::ShaderDataType::Float3: return GL_FLOAT;
			case DEngine::ShaderDataType::Float4: return GL_FLOAT;
			case DEngine::ShaderDataType::Mat3:   return GL_FLOAT;
			case DEngine::ShaderDataType::Mat4:   return GL_FLOAT;
			case DEngine::ShaderDataType::Int:    return GL_INT;
			case DEngine::ShaderDataType::Int2:   return GL_INT;
			case DEngine::ShaderDataType::Int3:   return GL_INT;
			case DEngine::ShaderDataType::Int4:   return GL_INT;
			case DEngine::ShaderDataType::Bool:   return GL_BOOL;
        }

        D_CORE_ASSERT(false, "Can't convert ShaderDataType to OpenGL type");
        return 0;
    }

	OpenGLVertexArray::OpenGLVertexArray()
	{
		glCreateVertexArrays(1, &m_RendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(m_RendererID);
	}

	void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertBuffer)
	{
		glBindVertexArray(m_RendererID);
		vertBuffer->Bind();

		D_CORE_ASSERT(vertBuffer->GetLayout().GetElements().size(), "VertexArray::AddVertexBuffer(...): Vertex Buffer has no layout");

        uint32_t ind = 0;
        for (const auto& elem : vertBuffer->GetLayout())
        {
			glEnableVertexAttribArray(ind);
			glVertexAttribPointer(
                ind, 
                elem.GetComponentCount(), 
                ShaderDataTypeToOpenGLType(elem.Type), 
                elem.IsNorm ? GL_TRUE : GL_FALSE, 
                vertBuffer->GetLayout().GetStride(),
                (const void*)elem.Offset
			);

			ind++;
        }

		m_VertexBuffers.push_back(vertBuffer);
	}

	void OpenGLVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indBuffer)
	{
		glBindVertexArray(m_RendererID);
		indBuffer->Bind();

		m_IndexBuffer = indBuffer;
	}
}