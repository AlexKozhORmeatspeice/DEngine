#include "dpch.h"
#include "RenderCommand.h"

//TODO: TEMP
#include "DEngine/Platform/OpenGL/OpenGLRendererAPI.h"
#include "DEngine/Platform/Vulkan/VulkanRendererAPI.h"

namespace DEngine
{
	static RendererAPI* CreateRendererAPI()
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::OpenGL: return new OpenGLRendererAPI();
			case RendererAPI::API::Vulkan: return new VulkanRendererAPI();
			case RendererAPI::API::None: D_CORE_ASSERT(false, "None API is not supported"); break;
		}

		D_CORE_ASSERT(false, "Current Renderer API doesn't support a renderer");
		return nullptr;
	}

	RendererAPI* RenderCommand::s_RendererAPI = CreateRendererAPI();
}