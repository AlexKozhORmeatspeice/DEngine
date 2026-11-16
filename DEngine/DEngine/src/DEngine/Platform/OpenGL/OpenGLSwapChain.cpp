#include "dpch.h"
#include "OpenGLSwapChain.h"

namespace DEngine
{
	OpenGLSwapChain::OpenGLSwapChain()
	{
	}

	OpenGLSwapChain::OpenGLSwapChain(RenderDevice& device, uint32_t width, uint32_t height)
		: SwapChain(device, width, height)
	{
	}
}
