#include "dpch.h"
#include "VulkanHelper.h"

namespace DEngine
{
	VkBool32 VulkanHelper::Bool(bool value)
	{
		return value ? VK_TRUE : VK_FALSE;
	}

	VkFormat VulkanHelper::Format(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Float: return VK_FORMAT_R32_SFLOAT;
			case ShaderDataType::Float2: return VK_FORMAT_R32G32_SFLOAT;
			case ShaderDataType::Float3: return VK_FORMAT_R32G32B32_SFLOAT;
			case ShaderDataType::Float4: return VK_FORMAT_R32G32B32A32_SFLOAT;
			case ShaderDataType::Mat3: return VK_FORMAT_R32G32B32_SFLOAT;
			case ShaderDataType::Mat4: return VK_FORMAT_R32G32B32A32_SFLOAT;
			case ShaderDataType::Int: return VK_FORMAT_R32_SINT;
			case ShaderDataType::Int2: return VK_FORMAT_R32G32_SINT;
			case ShaderDataType::Int3: return VK_FORMAT_R32G32B32_SINT;
			case ShaderDataType::Int4: return VK_FORMAT_R32G32B32A32_SINT;
			case ShaderDataType::Bool: return VK_FORMAT_R8_UINT;
			case ShaderDataType::None: break;
		}

		D_CORE_ASSERT(false, "Unsupported shader data type");
		return VK_FORMAT_UNDEFINED;
	}

	VkVertexInputBindingDescription VulkanHelper::VertexBinding(const BufferLayout& layout, uint32_t binding)
	{
		VkVertexInputBindingDescription result{};
		result.binding = binding;
		result.stride = layout.GetStride();
		result.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return result;
	}

	std::vector<VkVertexInputAttributeDescription> VulkanHelper::VertexAttributes(const BufferLayout& layout, uint32_t binding)
	{
		std::vector<VkVertexInputAttributeDescription> result;
		result.reserve(layout.GetElements().size());

		uint32_t location = 0;
		for (const BufferElement& element : layout)
		{
			VkVertexInputAttributeDescription attribute{};
			attribute.location = location;
			attribute.binding = binding;
			attribute.format = Format(element.Type);
			attribute.offset = element.Offset;
			result.push_back(attribute);
			location++;
		}

		return result;
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

	VkShaderStageFlags VulkanHelper::ShaderStage(uint32_t stageFlags)
	{
		VkShaderStageFlags result = 0;
		if (stageFlags & DShaderStage::Vertex)
			result |= VK_SHADER_STAGE_VERTEX_BIT;
		if (stageFlags & DShaderStage::Fragment)
			result |= VK_SHADER_STAGE_FRAGMENT_BIT;
		if (stageFlags & DShaderStage::Geometry)
			result |= VK_SHADER_STAGE_GEOMETRY_BIT;
		if (stageFlags & DShaderStage::TessellationControl)
			result |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		if (stageFlags & DShaderStage::TessellationEvaluation)
			result |= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		if (stageFlags & DShaderStage::Compute)
			result |= VK_SHADER_STAGE_COMPUTE_BIT;
		return result;
	}

	VkDescriptorType VulkanHelper::DescriptorType(DDescriptorType type)
	{
		switch (type)
		{
		case DDescriptorType::UniformBuffer: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		case DDescriptorType::CombinedImageSampler: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		}

		D_CORE_ASSERT(false, "Unsupported descriptor type");
		return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	}

	VkDescriptorSetLayoutBinding VulkanHelper::DescriptorBinding(const DDescriptorBinding& binding)
	{
		VkDescriptorSetLayoutBinding result{};
		result.binding = binding.binding;
		result.descriptorType = DescriptorType(binding.type);
		result.descriptorCount = binding.count;
		result.stageFlags = ShaderStage(binding.stageFlags);
		result.pImmutableSamplers = nullptr;
		return result;
	}

	VkPushConstantRange VulkanHelper::PushConstantRange(const DPushConstantRange& range)
	{
		VkPushConstantRange result{};
		result.stageFlags = ShaderStage(range.stageFlags);
		result.offset = range.offset;
		result.size = range.size;
		return result;
	}

	VkPipelineLayoutCreateInfo VulkanHelper::PipelineLayout(const PipelineLayoutState& state, const VkDescriptorSetLayout* setLayouts, const VkPushConstantRange* pushConstants)
	{
		VkPipelineLayoutCreateInfo result{};
		result.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		result.flags = static_cast<VkPipelineLayoutCreateFlags>(state.flags);
		result.setLayoutCount = static_cast<uint32_t>(state.setLayouts.size());
		result.pSetLayouts = setLayouts;
		result.pushConstantRangeCount = static_cast<uint32_t>(state.pushConstantRanges.size());
		result.pPushConstantRanges = pushConstants;
		return result;
	}

	VkCommandBufferLevel VulkanHelper::CommandBufferLevel(DCommandBufferLevel level)
	{
		switch (level)
		{
		case DCommandBufferLevel::Primary: return VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		case DCommandBufferLevel::Secondary: return VK_COMMAND_BUFFER_LEVEL_SECONDARY;
		}

		D_CORE_ASSERT(false, "Unsupported command buffer level");
		return VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	}

	VkCommandBufferBeginInfo VulkanHelper::CommandBufferBegin(const CommandBufferState& state)
	{
		VkCommandBufferBeginInfo result{};
		result.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		if (state.oneTimeSubmit)
			result.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		return result;
	}

	VkClearValue VulkanHelper::ClearValue(const DClearValue& value)
	{
		VkClearValue result{};
		if (value.type == DClearType::DepthStencil)
		{
			result.depthStencil.depth = value.depth;
			result.depthStencil.stencil = value.stencil;
			return result;
		}

		result.color.float32[0] = value.color[0];
		result.color.float32[1] = value.color[1];
		result.color.float32[2] = value.color[2];
		result.color.float32[3] = value.color[3];
		return result;
	}

	VkSubpassContents VulkanHelper::SubpassContents(DSubpassContents contents)
	{
		switch (contents)
		{
		case DSubpassContents::Inline: return VK_SUBPASS_CONTENTS_INLINE;
		case DSubpassContents::Secondary: return VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS;
		}

		D_CORE_ASSERT(false, "Unsupported subpass contents");
		return VK_SUBPASS_CONTENTS_INLINE;
	}

	VkRenderPassBeginInfo VulkanHelper::RenderPassBegin(VkRenderPass renderPass, VkFramebuffer framebuffer, const RenderPassState& state, const VkClearValue* clearValues)
	{
		VkRenderPassBeginInfo result{};
		result.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		result.renderPass = renderPass;
		result.framebuffer = framebuffer;
		result.renderArea.offset = { state.offsetX, state.offsetY };
		result.renderArea.extent = { state.width, state.height };
		result.clearValueCount = static_cast<uint32_t>(state.clearValues.size());
		result.pClearValues = clearValues;
		return result;
	}
}
