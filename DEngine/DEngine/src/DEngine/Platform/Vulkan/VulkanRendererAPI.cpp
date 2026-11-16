#include "dpch.h"
#include "VulkanRendererAPI.h"

namespace DEngine
{
	void VulkanRendererAPI::Init()
	{
	}

	void VulkanRendererAPI::SetClearColor(const glm::vec4& color)
	{
	}

	void VulkanRendererAPI::Clear()
	{
	}

	void VulkanRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
	}

	void VulkanRendererAPI::DrawIndexed(const DEngine::Ref<VertexArray> vertArr)
	{
		if (!vertArr || !vertArr->GetIndexBuffer())
			return;
	}

	void VulkanRendererAPI::DrawIndexedWireframe(const DEngine::Ref<VertexArray> vertArr)
	{
		if (!vertArr || !vertArr->GetIndexBuffer())
			return;
	}
}
