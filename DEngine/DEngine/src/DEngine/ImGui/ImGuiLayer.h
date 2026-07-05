#pragma once

#include "DEngine/Core.h"
#include "DEngine/Layer/Layer.h"

#include "DEngine/Events/KeyEvent.h"
#include "DEngine/Events/MouseEvent.h"
#include "DEngine/Events/ApplicationEvent.h"

namespace DEngine
{
	class D_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		virtual ~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnImGuiRenderer() override;

		void Begin();
		void End();

		virtual void OnEvent(Event& event);

		void SetBlockEvents(bool block) { m_BlockEvents = block; }
	private:
		bool m_BlockEvents = true;
		float m_Time = 0.0f;
	};
}
