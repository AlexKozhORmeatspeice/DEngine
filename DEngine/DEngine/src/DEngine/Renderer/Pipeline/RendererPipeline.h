#pragma once

#include "string"

namespace DEngine
{
	struct PipelineState
	{
		std::string vertFilepath;
		std::string fragFilepath;
	};

	class RendererPipeline
	{
	public:
		RendererPipeline();
		RendererPipeline(const PipelineState& state);

	private:
		PipelineState m_State;
	};
}
