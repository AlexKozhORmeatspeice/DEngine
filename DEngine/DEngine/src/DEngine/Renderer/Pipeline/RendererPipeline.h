#pragma once

#include "DEngine/Core.h"
#include "DEngine/Renderer/Pipeline/PipelineState.h"
#include "DEngine/Renderer/Device/RenderDevice.h"
#include "DEngine/Renderer/Shader/Shader.h"

namespace DEngine
{
	class D_API RendererPipeline
	{
	public:
		virtual ~RendererPipeline() = default;

		static Ref<RendererPipeline> Create(Ref<Shader> shader, RenderDevice& device, const PipelineState& state);

		RendererPipeline(const RendererPipeline&) = delete;
		void operator=(const RendererPipeline&) = delete;

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
