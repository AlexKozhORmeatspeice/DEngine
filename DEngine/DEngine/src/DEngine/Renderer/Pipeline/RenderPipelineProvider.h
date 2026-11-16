#pragma once
#include "RendererPipeline.h"
#include "unordered_map"

namespace DEngine
{
	class RenderPipelineProvider
	{
	public:
		void Init();
		void Shutdown();

		RendererPipeline& GetOrCreate(const PipelineState& state);
	private:
		std::unordered_map<std::string, RendererPipeline> m_Pipelines;
	};
}
