#include "dpch.h"
#include "DescriptorSet.h"

#include "DEngine/Renderer/Renderer.h"
#include "DEngine/Platform/OpenGL/OpenGLDescriptorSet.h"
#include "DEngine/Platform/Vulkan/VulkanDescriptorSet.h"

namespace DEngine
{
	Ref<DescriptorSet> DescriptorSet::Create(const Ref<PipelineLayout>& layout, uint32_t setIndex)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::OpenGL: return CreateRef<OpenGLDescriptorSet>(layout, setIndex);
			case RendererAPI::API::Vulkan: return CreateRef<VulkanDescriptorSet>(layout, setIndex);
			case RendererAPI::API::None: D_CORE_ASSERT(false, "None API is not supported");
			default:
				break;
		}

		D_CORE_ASSERT(false, "Current Renderer API doesn't support a descriptor set");
		return nullptr;
	}
}
