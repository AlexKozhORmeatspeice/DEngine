#pragma once

#include "DEngine/Core.h"
#include "DEngine/Renderer/Pipeline/PipelineState.h"
#include "DEngine/Renderer/Device/RenderDevice.h"
#include "DEngine/Renderer/Shader/Shader.h"

namespace DEngine
{
	class CommandBuffer;
	class D_API RendererPipeline
	{
	public:
		virtual ~RendererPipeline() = default;
		RendererPipeline(const RendererPipeline&) = delete;
		void operator=(const RendererPipeline&) = delete;

		static Ref<RendererPipeline> Create(Ref<Shader> shader, RenderDevice& device, const PipelineState& state);

		virtual void Bind(CommandBuffer& commandBuffer) = 0;
		virtual void SetViewport(CommandBuffer& commandBuffer, float x, float y, float width, float height) = 0;

		const PipelineState& GetState() const { return m_State; }

		static PipelineState CreateDefaultPipelineState(uint32_t width, uint32_t height);

	protected:
		RendererPipeline() = default;
		explicit RendererPipeline(RenderDevice& device, const PipelineState& state)
			: m_State(state)
		{
		}

		PipelineState m_State;
	};
}
