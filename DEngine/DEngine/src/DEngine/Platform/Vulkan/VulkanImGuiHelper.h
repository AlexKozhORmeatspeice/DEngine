#pragma once

namespace DEngine
{
	class VulkanImGuiHelper
	{
	public:
		static void Init();
		static void Shutdown();
		static void NewFrame();
		static void Render();
		static void OnResize(uint32_t imageCount);
		static void* RegisterTexture(void* imageView, void* sampler);
		static bool IsReady() { return s_Ready; }

	private:
		static bool s_Ready;
	};
}
