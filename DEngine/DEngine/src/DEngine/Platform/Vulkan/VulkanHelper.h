#pragma once

#include "DEngine/Renderer/Pipeline/PipelineLayout.h"
#include "DEngine/Renderer/Pipeline/PipelineState.h"
#include "DEngine/Renderer/Pipeline/RenderPass.h"
#include "DEngine/Renderer/CommandBuffer.h"

#include "vulkan/vulkan.h"

#include "vector"

namespace DEngine
{
	class VulkanHelper
	{
	public:
		static VkBool32 Bool(bool value);

		static VkFormat Format(ShaderDataType type);
		static VkVertexInputBindingDescription VertexBinding(const BufferLayout& layout, uint32_t binding = 0);
		static std::vector<VkVertexInputAttributeDescription> VertexAttributes(const BufferLayout& layout, uint32_t binding = 0);

		static VkPrimitiveTopology Topology(DPrimitiveTopology topology);
		static VkPolygonMode PolygonMode(DPolygonMode mode);
		static VkCullModeFlags CullMode(DCullMode mode);
		static VkFrontFace FrontFace(DFrontFace face);
		static VkSampleCountFlagBits SampleCount(DSampleCount count);
		static VkBlendFactor BlendFactor(DBlendFactor factor);
		static VkBlendOp BlendOp(DBlendOp op);
		static VkLogicOp LogicOp(DLogicOp op);
		static VkCompareOp CompareOp(DCompareOp op);

		static VkViewport Viewport(const DViewport& viewport);
		static VkRect2D Scissor(const DScissor& scissor);
		static VkPipelineViewportStateCreateInfo ViewportState(const DViewportState& state, const VkViewport* viewports, const VkRect2D* scissors);
		static VkPipelineInputAssemblyStateCreateInfo InputAssembly(const DInputAssembly& state);
		static VkPipelineRasterizationStateCreateInfo Rasterization(const DRasterization& state);
		static VkPipelineMultisampleStateCreateInfo Multisample(const DMultisample& state);
		static VkPipelineColorBlendAttachmentState ColorBlendAttachment(const DColorBlendAttachment& state);
		static VkPipelineColorBlendStateCreateInfo ColorBlend(const DColorBlend& state, const VkPipelineColorBlendAttachmentState* attachments);
		static VkPipelineDepthStencilStateCreateInfo DepthStencil(const DDepthStencil& state);

		static VkShaderStageFlags ShaderStage(uint32_t stageFlags);
		static VkDescriptorType DescriptorType(DDescriptorType type);
		static VkDescriptorSetLayoutBinding DescriptorBinding(const DDescriptorBinding& binding);
		static VkPushConstantRange PushConstantRange(const DPushConstantRange& range);
		static VkPipelineLayoutCreateInfo PipelineLayout(const PipelineLayoutState& state, const VkDescriptorSetLayout* setLayouts, const VkPushConstantRange* pushConstants);

		static VkCommandBufferLevel CommandBufferLevel(DCommandBufferLevel level);
		static VkCommandBufferBeginInfo CommandBufferBegin(const CommandBufferState& state);
		static VkClearValue ClearValue(const DClearValue& value);
		static VkSubpassContents SubpassContents(DSubpassContents contents);
		static VkRenderPassBeginInfo RenderPassBegin(VkRenderPass renderPass, VkFramebuffer framebuffer, const RenderPassState& state, const VkClearValue* clearValues);
	};
}
