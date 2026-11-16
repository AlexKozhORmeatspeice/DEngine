#include "dpch.h"
#include "VulkanRendererPipeline.h"
#include "DEngine/Application.h"
#include "DEngine/Platform/Vulkan/VulkanRenderDevice.h"
#include "DEngine/Platform/Vulkan/VulkanHelper.h"
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

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexAttributeDescriptionCount = 0;
		vertexInputInfo.vertexBindingDescriptionCount = 0;
		vertexInputInfo.pVertexAttributeDescriptions = nullptr;
		vertexInputInfo.pVertexBindingDescriptions = nullptr;

		const VkViewport viewport = VulkanHelper::Viewport(m_State.viewport);
		const VkRect2D scissor = VulkanHelper::Scissor(m_State.scissor);
		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		const VkPipelineInputAssemblyStateCreateInfo inputAssembly = VulkanHelper::InputAssembly(m_State.inputAssembly);
		const VkPipelineRasterizationStateCreateInfo rasterization = VulkanHelper::Rasterization(m_State.rasterization);
		const VkPipelineMultisampleStateCreateInfo multisample = VulkanHelper::Multisample(m_State.multisample);
		const VkPipelineColorBlendAttachmentState colorBlendAttachment = VulkanHelper::ColorBlendAttachment(m_State.colorBlendAttachment);
		const VkPipelineColorBlendStateCreateInfo colorBlend = VulkanHelper::ColorBlend(m_State.colorBlend, &colorBlendAttachment);
		const VkPipelineDepthStencilStateCreateInfo depthStencil = VulkanHelper::DepthStencil(m_State.depthStencil);

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
		pipelineInfo.pDynamicState = nullptr;
		pipelineInfo.layout = VulkanHelper::PipelineLayout(m_State.pipelineLayout);
		pipelineInfo.renderPass = VulkanHelper::RenderPass(m_State.renderPass);
		pipelineInfo.subpass = m_State.subpass;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;

		if (pipelineInfo.layout != VK_NULL_HANDLE && pipelineInfo.renderPass != VK_NULL_HANDLE)
		{
			auto vkDevice = static_cast<VkDevice*>(device.GetNativeDevice());
			D_CORE_ASSERT(vkCreateGraphicsPipelines(*vkDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline) == VK_SUCCESS, "Failed to create graphics pipeline");
		}
	}

	void VulkanRendererPipeline::CreateShaderModule(const std::vector<uint32_t>& code, VkShaderModule* shaderModule)
	{
		auto device = static_cast<VkDevice*>(Application::Get().GetDevice().GetNativeDevice());

		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size() * sizeof(uint32_t);
		createInfo.pCode = code.data();

		D_CORE_ASSERT(vkCreateShaderModule(*device, &createInfo, nullptr, shaderModule) == VK_SUCCESS, "Failed to create shader module");
	}
}
