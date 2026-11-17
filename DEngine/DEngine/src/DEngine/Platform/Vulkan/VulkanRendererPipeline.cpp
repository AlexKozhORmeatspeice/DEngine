#include "dpch.h"
#include "VulkanRendererPipeline.h"
#include "DEngine/Platform/Vulkan/VulkanCommandBuffer.h"
#include "DEngine/Application.h"
#include "DEngine/Platform/Vulkan/VulkanRenderDevice.h"
#include "DEngine/Platform/Vulkan/VulkanHelper.h"
#include "DEngine/Platform/Vulkan/VulkanPipelineLayout.h"
#include "DEngine/Platform/Vulkan/VulkanRenderPass.h"
#include "DEngine/Platform/Vulkan/VulkanShader.h"

namespace DEngine
{
	//SETTINGS
	const int MAX_SHADER_COUNT = 2;
	const uint32_t VERTEX_SHADER_IND = 0;
	const uint32_t FRAGMENT_SHADER_IND = 1;
	
	//PROGRAM
	VulkanRendererPipeline::VulkanRendererPipeline()
	{
	}

	VulkanRendererPipeline::~VulkanRendererPipeline()
	{
		auto device = static_cast<VkDevice*>(Application::Get().GetDevice().GetNativeDevice());
		vkDestroyPipeline(*device, m_Pipeline, nullptr);
	}

	VulkanRendererPipeline::VulkanRendererPipeline(Ref<Shader> shader, RenderDevice& device, const PipelineState& state)
		: RendererPipeline(device, state)
	{
		//Устанавливаем шейдеры
		auto vkShader = CastRef<VulkanShader>(shader);
		const auto modulesMap = vkShader->GetShaderModules();

		VkPipelineShaderStageCreateInfo shaderStages[MAX_SHADER_COUNT]{};
		auto itVertex = modulesMap.find(ShaderType::Vertex);
		if (itVertex != modulesMap.end())
		{
			shaderStages[VERTEX_SHADER_IND].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderStages[VERTEX_SHADER_IND].stage = VK_SHADER_STAGE_VERTEX_BIT;
			shaderStages[VERTEX_SHADER_IND].module = itVertex->second;
			shaderStages[VERTEX_SHADER_IND].pName = "main";
			shaderStages[VERTEX_SHADER_IND].flags = 0;
			shaderStages[VERTEX_SHADER_IND].pNext = nullptr;
			shaderStages[VERTEX_SHADER_IND].pSpecializationInfo = nullptr;
		}

		auto itFrag = modulesMap.find(ShaderType::Fragment);
		if (itFrag != modulesMap.end())
		{
			shaderStages[FRAGMENT_SHADER_IND].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderStages[FRAGMENT_SHADER_IND].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			shaderStages[FRAGMENT_SHADER_IND].module = itFrag->second;
			shaderStages[FRAGMENT_SHADER_IND].pName = "main";
			shaderStages[FRAGMENT_SHADER_IND].flags = 0;
			shaderStages[FRAGMENT_SHADER_IND].pNext = nullptr;
			shaderStages[FRAGMENT_SHADER_IND].pSpecializationInfo = nullptr;
		}

		const VkVertexInputBindingDescription vertexBinding = VulkanHelper::VertexBinding(m_State.vertexLayout);
		const std::vector<VkVertexInputAttributeDescription> vertexAttributes = VulkanHelper::VertexAttributes(m_State.vertexLayout);
		const bool hasVertexLayout = !m_State.vertexLayout.GetElements().empty();

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = hasVertexLayout ? 1 : 0;
		vertexInputInfo.pVertexBindingDescriptions = hasVertexLayout ? &vertexBinding : nullptr;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttributes.size());
		vertexInputInfo.pVertexAttributeDescriptions = vertexAttributes.empty() ? nullptr : vertexAttributes.data();

		VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = 2;
		dynamicState.pDynamicStates = dynamicStates;

		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		const VkPipelineInputAssemblyStateCreateInfo inputAssembly = VulkanHelper::InputAssembly(m_State.inputAssembly);
		const VkPipelineRasterizationStateCreateInfo rasterization = VulkanHelper::Rasterization(m_State.rasterization);
		const VkPipelineMultisampleStateCreateInfo multisample = VulkanHelper::Multisample(m_State.multisample);
		const VkPipelineColorBlendAttachmentState colorBlendAttachment = VulkanHelper::ColorBlendAttachment(m_State.colorBlendAttachment);
		const VkPipelineColorBlendStateCreateInfo colorBlend = VulkanHelper::ColorBlend(m_State.colorBlend, &colorBlendAttachment);
		const VkPipelineDepthStencilStateCreateInfo depthStencil = VulkanHelper::DepthStencil(m_State.depthStencil);

		VkPipelineLayout layout = VK_NULL_HANDLE;
		VkRenderPass renderPass = VK_NULL_HANDLE;
		if (m_State.pipelineLayout)
			layout = static_cast<VulkanPipelineLayout*>(m_State.pipelineLayout.get())->GetNative();
		if (m_State.renderPass)
			renderPass = static_cast<VulkanRenderPass*>(m_State.renderPass.get())->GetNative();

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = MAX_SHADER_COUNT;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterization;
		pipelineInfo.pMultisampleState = &multisample;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.pColorBlendState = &colorBlend;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = layout;
		pipelineInfo.renderPass = renderPass;
		pipelineInfo.subpass = m_State.subpass;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;

		if (pipelineInfo.layout != VK_NULL_HANDLE && pipelineInfo.renderPass != VK_NULL_HANDLE)
		{
			auto vkDevice = static_cast<VkDevice*>(device.GetNativeDevice());
			D_CORE_ASSERT(vkCreateGraphicsPipelines(*vkDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline) == VK_SUCCESS, "Failed to create graphics pipeline");
		}
	}

	void VulkanRendererPipeline::Bind(CommandBuffer& commandBuffer)
	{
		vkCmdBindPipeline(static_cast<VulkanCommandBuffer&>(commandBuffer).GetNative(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);
	}

	void VulkanRendererPipeline::SetViewport(CommandBuffer& commandBuffer, float x, float y, float width, float height)
	{
		VkViewport viewport{};
		viewport.x = x;
		viewport.y = y;
		viewport.width = width;
		viewport.height = height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		scissor.offset = { static_cast<int32_t>(x), static_cast<int32_t>(y) };
		scissor.extent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

		VkCommandBuffer native = static_cast<VulkanCommandBuffer&>(commandBuffer).GetNative();
		vkCmdSetViewport(native, 0, 1, &viewport);
		vkCmdSetScissor(native, 0, 1, &scissor);
	}
}
