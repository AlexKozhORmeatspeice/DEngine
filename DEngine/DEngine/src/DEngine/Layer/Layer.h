#pragma once

#include "DEngine/Core.h"
#include "DEngine/Events/Event.h"

#include "DEngine/Core/Timestep.h"

namespace DEngine
{
	class D_API Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(const Timestep& ts) {}
		virtual void OnImGuiRenderer() {}
		virtual void OnEvent(Event& event) {}

		inline const std::string& GetName() const { return m_DebugName; }
	private:
		std::string m_DebugName;
	};
}
