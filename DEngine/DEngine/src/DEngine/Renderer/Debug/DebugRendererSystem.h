#pragma once

#include "DEngine/Scene/System.h"

namespace DEngine
{
	class DebugRendererSystem : public System
	{
	public:
		DebugRendererSystem() {}
		virtual ~DebugRendererSystem() {}

		virtual void Start() override {}
		virtual void OnUpdate(const Timestep& ts, const Scene* scene) override {}
		virtual void OnRender(const Timestep& ts, const Scene* scene) override;
		virtual void Shutdown() override {}
	};
}
