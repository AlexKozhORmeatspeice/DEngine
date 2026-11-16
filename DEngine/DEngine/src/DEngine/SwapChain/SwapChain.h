#pragma once

#include "DEngine/Core.h"
#include "DEngine/Renderer/Device/RenderDevice.h"

namespace DEngine
{
	class D_API SwapChain
	{
	public:
		virtual ~SwapChain() = default;

		static Scope<SwapChain> Create(RenderDevice& device, uint32_t width, uint32_t height);

		SwapChain(const SwapChain&) = delete;
		void operator=(const SwapChain&) = delete;

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }
		uint32_t GetImageCount() const { return m_ImageCount; }

		float GetExtentAspectRatio() const
		{
			return static_cast<float>(m_Width) / static_cast<float>(m_Height);
		}

	protected:
		SwapChain() = default;
		explicit SwapChain(RenderDevice& device, uint32_t width, uint32_t height)
			: m_Width(width)
			, m_Height(height)
		{
		}

		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
		uint32_t m_ImageCount = 0;
	};
}
