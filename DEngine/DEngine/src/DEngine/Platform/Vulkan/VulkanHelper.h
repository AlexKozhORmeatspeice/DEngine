#pragma once

#include "DEngine/Renderer/Pipeline/PipelineState.h"

#include "vulkan/vulkan.h"

namespace DEngine
{
	class VulkanHelper
	{
	public:
		static VkBool32 Bool(bool value);

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
		static VkPipelineLayout PipelineLayout(const DPipelineLayout& layout);
		static VkRenderPass RenderPass(const DRenderPass& renderPass);
	};
}
