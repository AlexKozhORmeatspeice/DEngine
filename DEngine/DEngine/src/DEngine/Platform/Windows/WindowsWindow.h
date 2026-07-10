#pragma once
#include "DEngine/Window.h"
#include "DEngine/Renderer/GraphicsContext.h"

class GLFWwindow;

namespace DEngine
{
	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps&);
		virtual ~WindowsWindow();
		
		void OnUpdate() override;

		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }
		
		inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

		inline virtual void* GetNativeWindow() const
		{
			return m_Window;
		}

	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
	private:
		GLFWwindow* m_Window;
		GraphicsContext* m_Context;

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			 bool IsFullscreen = false;
			int WindowedPosX = 0;
			int WindowedPosY = 0;
			int WindowedWidth = 800;
			int WindowedHeight = 600;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};
}
