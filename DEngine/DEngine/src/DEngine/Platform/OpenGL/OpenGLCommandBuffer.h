#pragma once

#include "DEngine/Renderer/CommandBuffer.h"

namespace DEngine
{
	class D_API OpenGLCommandBuffer : public CommandBuffer
	{
	public:
		explicit OpenGLCommandBuffer(RenderDevice& device, const CommandBufferState& state);
		~OpenGLCommandBuffer() override = default;

		void Reset() override;
		void Begin() override;
		void End() override;
	};
}
