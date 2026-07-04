#pragma once

#include "DEngine/Core.h"
#include "stdint.h"
#include "vector"

namespace DEngine
{
	enum class ShaderDataType
	{
		None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
	};

	static uint32_t ShaderDataSize(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::None:   return 0;
			case ShaderDataType::Float:  return 4;
			case ShaderDataType::Float2: return 8;
			case ShaderDataType::Float3: return 12;
			case ShaderDataType::Float4: return 16;
			case ShaderDataType::Mat3:   return 36;
			case ShaderDataType::Mat4:   return 64;
			case ShaderDataType::Int:    return 4;
			case ShaderDataType::Int2:   return 8;
			case ShaderDataType::Int3:   return 12;
			case ShaderDataType::Int4:   return 16;
			case ShaderDataType::Bool:   return 1;
		}

		D_CORE_ASSERT(false, "ShaderDataSize(ShaderDataType): Unkown ShaderDataType");
		return 0;
	}

	struct D_API BufferElement
	{
		std::string Name;
		ShaderDataType Type;
		uint32_t Offset;
		uint32_t Size;

		bool IsNorm;
		BufferElement()
			: Name(""), Type(ShaderDataType::None), Size(0), Offset(0), IsNorm(false)
		{}

		BufferElement(ShaderDataType type, const std::string& name, bool isNorm = false)
			: Name(name), Type(type), Size(ShaderDataSize(type)), Offset(0), IsNorm(isNorm)
		{ 
		}

		uint32_t GetComponentCount() const
		{
			switch (Type)
			{
				case DEngine::ShaderDataType::None:   return 0;
				case DEngine::ShaderDataType::Float:  return 1;
				case DEngine::ShaderDataType::Float2: return 2;
				case DEngine::ShaderDataType::Float3: return 3;
				case DEngine::ShaderDataType::Float4: return 4;
				case DEngine::ShaderDataType::Mat3:   return 9;
				case DEngine::ShaderDataType::Mat4:   return 12;
				case DEngine::ShaderDataType::Int:    return 1;
				case DEngine::ShaderDataType::Int2:   return 2;
				case DEngine::ShaderDataType::Int3:   return 3;
				case DEngine::ShaderDataType::Int4:   return 4;
				case DEngine::ShaderDataType::Bool:   return 1;
				default:
					break;
			}
		}
	};

	class D_API BufferLayout
	{
	public:
		BufferLayout(const std::initializer_list<BufferElement>& elems)
			: m_Elements(elems)
		{
			CalculateOffsetAndStride();
		}
		BufferLayout()
		{
			m_Elements.clear();
		}

		inline uint32_t GetStride() const { return m_Stride; }
		inline const std::vector<BufferElement>& GetElements() const { return m_Elements; }

		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

	private:
		void CalculateOffsetAndStride()
		{
			uint32_t offset = 0;
			m_Stride = 0;

			for (auto& elem : m_Elements)
			{
				elem.Offset = offset;
				offset += elem.Size;
				m_Stride += elem.Size;
			}
		}

		std::vector<BufferElement> m_Elements;
		uint32_t m_Stride = 0;
	};

	class D_API VertexBuffer
	{
	public:
		virtual ~VertexBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetLayout(const BufferLayout& layout) = 0;
		virtual const BufferLayout& GetLayout() const = 0;

		static Ref<VertexBuffer> Create(float* verts, uint32_t size);
	protected:
		VertexBuffer() {}
	};

	class D_API IndexBuffer
	{
	public:
		virtual ~IndexBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual uint32_t GetCount() const = 0;

		static Ref<IndexBuffer> Create(uint32_t* inds, uint32_t count);
	protected:
		IndexBuffer() {}
	};
}
