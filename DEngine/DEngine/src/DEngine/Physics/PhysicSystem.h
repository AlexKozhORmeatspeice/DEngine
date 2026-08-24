#pragma once

#include "DEngine/Scene/System.h"

namespace DEngine
{
	class PhysicsSystem : public System
	{
	public:
		PhysicsSystem() {}
		virtual ~PhysicsSystem() {}

		virtual void Start() override {}
		virtual void OnUpdate(const Timestep& ts, const Scene* scene) override;
		virtual void OnRender(const Timestep& ts, const Scene* scene) override {}
		virtual void Shutdown() override {}
	};
}
