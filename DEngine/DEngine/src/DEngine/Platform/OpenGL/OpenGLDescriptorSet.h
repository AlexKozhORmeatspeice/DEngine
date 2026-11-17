#pragma once

#include "DEngine/Renderer/Pipeline/DescriptorSet.h"

#include "unordered_map"
#include "vector"

namespace DEngine
{
	class D_API OpenGLDescriptorSet : public DescriptorSet
	{
	public:
		OpenGLDescriptorSet(const Ref<PipelineLayout>& layout, uint32_t setIndex);

		void WriteUniform(uint32_t binding, const Ref<UniformBuffer>& buffer) override;
		void WriteTexture(uint32_t binding, const Ref<Texture>& texture) override;
		void Bind(CommandBuffer& commandBuffer, const Ref<PipelineLayout>& layout) override;

	private:
		uint32_t m_SetIndex = 0;
		std::unordered_map<uint32_t, Ref<UniformBuffer>> m_Uniforms;
		std::unordered_map<uint32_t, Ref<Texture>> m_Textures;
	};
}
