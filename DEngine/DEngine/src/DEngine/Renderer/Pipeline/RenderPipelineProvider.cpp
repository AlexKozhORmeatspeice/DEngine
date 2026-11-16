#include "dpch.h"
#include "RenderPipelineProvider.h"

namespace DEngine
{
	void RenderPipelineProvider::Init()
	{
	}

	void RenderPipelineProvider::Shutdown()
	{
		m_Pipelines.clear();
	}

	RendererPipeline& RenderPipelineProvider::GetOrCreate(Ref<Shader> shader, RenderDevice& device, const PipelineState& state)
	{
		const std::string key = "";
		auto it = m_Pipelines.find(key);
		if (it != m_Pipelines.end() && it->second)
			return *it->second;

		Ref<RendererPipeline> pipeline = RendererPipeline::Create(shader, device, state);
		D_CORE_ASSERT(pipeline, "Renderer pipeline was not created");
		RendererPipeline& created = *pipeline;
		m_Pipelines[key] = std::move(pipeline);
		return created;
	}
}
