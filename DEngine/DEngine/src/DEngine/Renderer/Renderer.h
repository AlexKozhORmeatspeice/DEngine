#pragma once

#include "DEngine/Core.h"

namespace DEngine
{
	enum class RendererAPI
	{
		None = 0, OpenGL = 1
	};

	class D_API Renderer
	{
	public:
		inline static RendererAPI GetAPI() { return s_RendererAPI; }
	
	private:
		static RendererAPI s_RendererAPI;
	};

}
