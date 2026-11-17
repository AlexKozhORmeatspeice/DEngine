#include "dpch.h"
#include "PipelineLayout.h"

#include "DEngine/Renderer/Renderer.h"
#include "DEngine/Platform/OpenGL/OpenGLPipelineLayout.h"
#include "DEngine/Platform/Vulkan/VulkanPipelineLayout.h"

namespace DEngine
{
	Ref<PipelineLayout> PipelineLayout::Create(RenderDevice& device, const PipelineLayoutState& state)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::OpenGL: return CreateRef<OpenGLPipelineLayout>(device, state);
			case RendererAPI::API::Vulkan: return CreateRef<VulkanPipelineLayout>(device, state);
			case RendererAPI::API::None: D_CORE_ASSERT(false, "None API is not supported");
			default:
				break;
		}

		D_CORE_ASSERT(false, "Current Renderer API doesn't support a pipeline layout");
		return nullptr;
	}
}
