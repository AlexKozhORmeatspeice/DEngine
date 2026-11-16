#include "dpch.h"
#include "RenderPipelineProvider.h"

namespace DEngine
{
	void RenderPipelineProvider::Init()
	{
		
	}

	void RenderPipelineProvider::Shutdown()
	{
		
	}

	RendererPipeline& RenderPipelineProvider::GetOrCreate(const PipelineState& state)
	{
		RendererPipeline pipeline;

		//WIP: в будущем эти объекты будут храниться в unordered_map

		return pipeline;
	}
}