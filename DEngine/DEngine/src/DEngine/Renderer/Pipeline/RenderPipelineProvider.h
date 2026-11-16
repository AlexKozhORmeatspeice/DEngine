#pragma once

#include "DEngine/Core.h"
#include "RendererPipeline.h"
#include "DEngine/Renderer/Device/RenderDevice.h"
#include "unordered_map"
#include "DEngine/Renderer/Shader/Shader.h"

namespace DEngine
{
	class RenderPipelineProvider
	{
	public:
		void Init();
		void Shutdown();

		RendererPipeline& GetOrCreate(Ref<Shader> shader, RenderDevice& device, const PipelineState& state);
	private:
		std::unordered_map<std::string, Ref<RendererPipeline>> m_Pipelines;
	};
}
