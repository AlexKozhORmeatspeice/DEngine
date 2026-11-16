#include "dpch.h"
#include "VertexArray.h"
#include "DEngine/Application.h"
#include "DEngine/Renderer/Renderer.h"
#include "DEngine/Platform/OpenGL/OpenGLVertexArray.h"
#include "DEngine/Platform/Vulkan/VulkanVertexArray.h"

namespace DEngine
{
	Ref<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::OpenGL: return std::make_shared<OpenGLVertexArray>();
			case RendererAPI::API::Vulkan: return std::make_shared<VulkanVertexArray>();
			case RendererAPI::API::None: D_CORE_ASSERT(false, "None API is not supported");
			default:
				break;
		}

		D_CORE_ASSERT(false, "Current Renderer API doesn't support vertex array");
		return nullptr;
	}
}