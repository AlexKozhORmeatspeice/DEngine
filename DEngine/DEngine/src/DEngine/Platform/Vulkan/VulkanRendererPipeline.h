#pragma once

#include "DEngine/Renderer/Pipeline/RendererPipeline.h"
#include "vulkan/vulkan.h"

namespace DEngine
{
	struct PipelineConfigInfo
	{
		//
	};

	class D_API VulkanRendererPipeline : public RendererPipeline
	{
	public:
		VulkanRendererPipeline();
		explicit VulkanRendererPipeline(Ref<Shader> shader, RenderDevice& device, const PipelineState& state);
		~VulkanRendererPipeline() override;

		virtual void Bind(CommandBuffer& commandBuffer) override;
		virtual void SetViewport(CommandBuffer& commandBuffer, float x, float y, float width, float height) override;

	private:

		VkPipeline m_Pipeline = VK_NULL_HANDLE;
		VkShaderModule m_VertShaderModule;
		VkShaderModule m_FragShaderModule;
	};
}
