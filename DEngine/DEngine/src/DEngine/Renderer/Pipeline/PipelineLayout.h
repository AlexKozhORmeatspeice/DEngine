#pragma once

#include "DEngine/Core.h"
#include "DEngine/Renderer/Device/RenderDevice.h"
#include "DEngine/Renderer/CommandBuffer.h"

#include "vector"

namespace DEngine
{
	struct DShaderStage
	{
		static constexpr uint32_t Vertex = 1 << 0;
		static constexpr uint32_t Fragment = 1 << 1;
		static constexpr uint32_t Geometry = 1 << 2;
		static constexpr uint32_t TessellationControl = 1 << 3;
		static constexpr uint32_t TessellationEvaluation = 1 << 4;
		static constexpr uint32_t Compute = 1 << 5;
	};

	enum class DDescriptorType
	{
		UniformBuffer,
		CombinedImageSampler
	};

	struct DDescriptorBinding
	{
		uint32_t binding = 0;
		DDescriptorType type = DDescriptorType::UniformBuffer;
		uint32_t stageFlags = DShaderStage::Vertex;
		uint32_t count = 1;
	};

	struct DDescriptorSetLayout
	{
		std::vector<DDescriptorBinding> bindings;
	};

	struct DPushConstantRange
	{
		uint32_t stageFlags = DShaderStage::Vertex;
		uint32_t offset = 0;
		uint32_t size = 0;
	};

	struct PipelineLayoutState
	{
		uint32_t flags = 0;
		std::vector<DDescriptorSetLayout> setLayouts;
		std::vector<DPushConstantRange> pushConstantRanges;
	};

	class D_API PipelineLayout
	{
	public:
		virtual ~PipelineLayout() = default;

		static Ref<PipelineLayout> Create(RenderDevice& device, const PipelineLayoutState& state);

		PipelineLayout(const PipelineLayout&) = delete;
		void operator=(const PipelineLayout&) = delete;

		const PipelineLayoutState& GetState() const { return m_State; }
		virtual void PushConstant(CommandBuffer& commandBuffer, uint32_t stageFlags, uint32_t offset, uint32_t size, const void* data) = 0;

	protected:
		PipelineLayout() = default;
		explicit PipelineLayout(RenderDevice& device, const PipelineLayoutState& state)
			: m_State(state)
		{
		}

		PipelineLayoutState m_State;
	};
}
