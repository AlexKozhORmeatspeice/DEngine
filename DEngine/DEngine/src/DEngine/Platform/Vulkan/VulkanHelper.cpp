#include "dpch.h"
#include "VulkanHelper.h"

namespace DEngine
{
	VkBool32 VulkanHelper::Bool(bool value)
	{
		return value ? VK_TRUE : VK_FALSE;
	}

	VkPrimitiveTopology VulkanHelper::Topology(DPrimitiveTopology topology)
	{
		switch (topology)
		{
		case DPrimitiveTopology::TriangleList: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		}

		D_CORE_ASSERT(false, "Unsupported primitive topology");
		return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	}

	VkPolygonMode VulkanHelper::PolygonMode(DPolygonMode mode)
	{
		switch (mode)
		{
		case DPolygonMode::Fill: return VK_POLYGON_MODE_FILL;
		}

		D_CORE_ASSERT(false, "Unsupported polygon mode");
		return VK_POLYGON_MODE_FILL;
	}

	VkCullModeFlags VulkanHelper::CullMode(DCullMode mode)
	{
		switch (mode)
		{
		case DCullMode::None: return VK_CULL_MODE_NONE;
		}

		D_CORE_ASSERT(false, "Unsupported cull mode");
		return VK_CULL_MODE_NONE;
	}

	VkFrontFace VulkanHelper::FrontFace(DFrontFace face)
	{
		switch (face)
		{
		case DFrontFace::Clockwise: return VK_FRONT_FACE_CLOCKWISE;
		}

		D_CORE_ASSERT(false, "Unsupported front face");
		return VK_FRONT_FACE_CLOCKWISE;
	}

	VkSampleCountFlagBits VulkanHelper::SampleCount(DSampleCount count)
	{
		switch (count)
		{
		case DSampleCount::Count1: return VK_SAMPLE_COUNT_1_BIT;
		}

		D_CORE_ASSERT(false, "Unsupported sample count");
		return VK_SAMPLE_COUNT_1_BIT;
	}

	VkBlendFactor VulkanHelper::BlendFactor(DBlendFactor factor)
	{
		switch (factor)
		{
		case DBlendFactor::Zero: return VK_BLEND_FACTOR_ZERO;
		case DBlendFactor::One: return VK_BLEND_FACTOR_ONE;
		}

		D_CORE_ASSERT(false, "Unsupported blend factor");
		return VK_BLEND_FACTOR_ZERO;
	}

	VkBlendOp VulkanHelper::BlendOp(DBlendOp op)
	{
		switch (op)
		{
		case DBlendOp::Add: return VK_BLEND_OP_ADD;
		}

		D_CORE_ASSERT(false, "Unsupported blend op");
		return VK_BLEND_OP_ADD;
	}

	VkLogicOp VulkanHelper::LogicOp(DLogicOp op)
	{
		switch (op)
		{
		case DLogicOp::Copy: return VK_LOGIC_OP_COPY;
		}

		D_CORE_ASSERT(false, "Unsupported logic op");
		return VK_LOGIC_OP_COPY;
	}

	VkCompareOp VulkanHelper::CompareOp(DCompareOp op)
	{
		switch (op)
		{
		case DCompareOp::Less: return VK_COMPARE_OP_LESS;
		}

		D_CORE_ASSERT(false, "Unsupported compare op");
		return VK_COMPARE_OP_LESS;
	}

	VkViewport VulkanHelper::Viewport(const DViewport& viewport)
	{
		VkViewport result{};
		result.x = viewport.x;
		result.y = viewport.y;
		result.width = viewport.width;
		result.height = viewport.height;
		result.minDepth = viewport.minDepth;
		result.maxDepth = viewport.maxDepth;
		return result;
	}

	VkRect2D VulkanHelper::Scissor(const DScissor& scissor)
	{
		VkRect2D result{};
		result.offset = { scissor.offset.x, scissor.offset.y };
		result.extent = { scissor.extent.width, scissor.extent.height };
		return result;
	}

	VkPipelineViewportStateCreateInfo VulkanHelper::ViewportState(const DViewportState& state, const VkViewport* viewports, const VkRect2D* scissors)
	{
		VkPipelineViewportStateCreateInfo result{};
		result.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		result.viewportCount = state.viewportCount;
		result.pViewports = viewports;
		result.scissorCount = state.scissorCount;
		result.pScissors = scissors;
		return result;
	}

	VkPipelineInputAssemblyStateCreateInfo VulkanHelper::InputAssembly(const DInputAssembly& state)
	{
		VkPipelineInputAssemblyStateCreateInfo result{};
		result.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		result.topology = Topology(state.topology);
		result.primitiveRestartEnable = Bool(state.primitiveRestart);
		return result;
	}

	VkPipelineRasterizationStateCreateInfo VulkanHelper::Rasterization(const DRasterization& state)
	{
		VkPipelineRasterizationStateCreateInfo result{};
		result.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		result.depthClampEnable = Bool(state.depthClamp);
		result.rasterizerDiscardEnable = Bool(state.rasterizerDiscard);
		result.polygonMode = PolygonMode(state.polygonMode);
		result.lineWidth = state.lineWidth;
		result.cullMode = CullMode(state.cullMode);
		result.frontFace = FrontFace(state.frontFace);
		result.depthBiasEnable = Bool(state.depthBias);
		result.depthBiasConstantFactor = state.depthBiasConstant;
		result.depthBiasClamp = state.depthBiasClamp;
		result.depthBiasSlopeFactor = state.depthBiasSlope;
		return result;
	}

	VkPipelineMultisampleStateCreateInfo VulkanHelper::Multisample(const DMultisample& state)
	{
		VkPipelineMultisampleStateCreateInfo result{};
		result.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		result.sampleShadingEnable = Bool(state.sampleShading);
		result.rasterizationSamples = SampleCount(state.rasterizationSamples);
		result.minSampleShading = state.minSampleShading;
		result.pSampleMask = state.sampleMask;
		result.alphaToCoverageEnable = Bool(state.alphaToCoverage);
		result.alphaToOneEnable = Bool(state.alphaToOne);
		return result;
	}

	VkPipelineColorBlendAttachmentState VulkanHelper::ColorBlendAttachment(const DColorBlendAttachment& state)
	{
		VkPipelineColorBlendAttachmentState result{};
		result.colorWriteMask = state.colorWriteMask;
		result.blendEnable = Bool(state.blend);
		result.srcColorBlendFactor = BlendFactor(state.srcColorBlendFactor);
		result.dstColorBlendFactor = BlendFactor(state.dstColorBlendFactor);
		result.colorBlendOp = BlendOp(state.colorBlendOp);
		result.srcAlphaBlendFactor = BlendFactor(state.srcAlphaBlendFactor);
		result.dstAlphaBlendFactor = BlendFactor(state.dstAlphaBlendFactor);
		result.alphaBlendOp = BlendOp(state.alphaBlendOp);
		return result;
	}

	VkPipelineColorBlendStateCreateInfo VulkanHelper::ColorBlend(const DColorBlend& state, const VkPipelineColorBlendAttachmentState* attachments)
	{
		VkPipelineColorBlendStateCreateInfo result{};
		result.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		result.logicOpEnable = Bool(state.logicOpEnable);
		result.logicOp = LogicOp(state.logicOp);
		result.attachmentCount = state.attachmentCount;
		result.pAttachments = attachments;
		result.blendConstants[0] = state.blendConstants[0];
		result.blendConstants[1] = state.blendConstants[1];
		result.blendConstants[2] = state.blendConstants[2];
		result.blendConstants[3] = state.blendConstants[3];
		return result;
	}

	VkPipelineDepthStencilStateCreateInfo VulkanHelper::DepthStencil(const DDepthStencil& state)
	{
		VkPipelineDepthStencilStateCreateInfo result{};
		result.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		result.depthTestEnable = Bool(state.depthTest);
		result.depthWriteEnable = Bool(state.depthWrite);
		result.depthCompareOp = CompareOp(state.depthCompareOp);
		result.depthBoundsTestEnable = Bool(state.depthBoundsTest);
		result.minDepthBounds = state.minDepthBounds;
		result.maxDepthBounds = state.maxDepthBounds;
		result.stencilTestEnable = Bool(state.stencilTest);
		result.front = {};
		result.back = {};
		return result;
	}

	VkPipelineLayout VulkanHelper::PipelineLayout(const DPipelineLayout& layout)
	{
		return reinterpret_cast<VkPipelineLayout>(layout.native);
	}

	VkRenderPass VulkanHelper::RenderPass(const DRenderPass& renderPass)
	{
		return reinterpret_cast<VkRenderPass>(renderPass.native);
	}
}
