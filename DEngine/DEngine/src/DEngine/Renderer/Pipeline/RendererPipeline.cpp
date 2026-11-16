#include "dpch.h"
#include "RendererPipeline.h"

#include "DEngine/Renderer/Renderer.h"
#include "DEngine/Platform/OpenGL/OpenGLRendererPipeline.h"
#include "DEngine/Platform/Vulkan/VulkanRendererPipeline.h"

namespace DEngine
{
	Ref<RendererPipeline> RendererPipeline::Create(Ref<Shader> shader, RenderDevice& device, const PipelineState& state)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::OpenGL: return CreateRef<OpenGLRendererPipeline>(shader, device, state);
			case RendererAPI::API::Vulkan: return CreateRef<VulkanRendererPipeline>(shader, device, state);
			case RendererAPI::API::None: D_CORE_ASSERT(false, "None API is not supported");
			default:
				break;
		}

		D_CORE_ASSERT(false, "Current Renderer API doesn't support a renderer pipeline");
		return nullptr;
	}
}
