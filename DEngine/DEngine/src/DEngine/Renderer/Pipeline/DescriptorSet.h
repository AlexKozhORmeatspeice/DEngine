#pragma once

#include "DEngine/Core.h"
#include "DEngine/Renderer/CommandBuffer.h"

namespace DEngine
{
	class PipelineLayout;
	class UniformBuffer;
	class Texture;

	class D_API DescriptorSet
	{
	public:
		virtual ~DescriptorSet() = default;

		static Ref<DescriptorSet> Create(const Ref<PipelineLayout>& layout, uint32_t setIndex);

		virtual void WriteUniform(uint32_t binding, const Ref<UniformBuffer>& buffer) = 0;
		virtual void WriteTexture(uint32_t binding, const Ref<Texture>& texture) = 0;
		virtual void Bind(CommandBuffer& commandBuffer, const Ref<PipelineLayout>& layout) = 0;
	};
}
