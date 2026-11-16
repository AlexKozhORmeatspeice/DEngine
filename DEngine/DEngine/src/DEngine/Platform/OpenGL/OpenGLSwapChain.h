#pragma once

#include "DEngine/SwapChain/SwapChain.h"

namespace DEngine
{
	class D_API OpenGLSwapChain : public SwapChain
	{
	public:
		OpenGLSwapChain();
		explicit OpenGLSwapChain(RenderDevice& device, uint32_t width, uint32_t height);
		~OpenGLSwapChain() override = default;
	};
}
