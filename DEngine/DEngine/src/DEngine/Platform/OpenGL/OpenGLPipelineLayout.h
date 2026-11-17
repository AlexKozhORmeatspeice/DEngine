#pragma once

#include "DEngine/Renderer/Pipeline/PipelineLayout.h"

namespace DEngine
{
	class D_API OpenGLPipelineLayout : public PipelineLayout
	{
	public:
		explicit OpenGLPipelineLayout(RenderDevice& device, const PipelineLayoutState& state);
		~OpenGLPipelineLayout() override = default;

		void PushConstant(CommandBuffer& commandBuffer, uint32_t stageFlags, uint32_t offset, uint32_t size, const void* data) override;
	};
}
