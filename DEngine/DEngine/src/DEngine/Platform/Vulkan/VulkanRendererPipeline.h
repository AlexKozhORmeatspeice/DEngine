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

	private:
		void CreateShaderModule(const std::vector<uint32_t>& code, VkShaderModule* shaderModule);

		VkPipeline m_Pipeline = VK_NULL_HANDLE;
		VkShaderModule m_VertShaderModule;
		VkShaderModule m_FragShaderModule;
	};
}
