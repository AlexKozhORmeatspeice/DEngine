#pragma once

#include "DEngine/Core.h"
#include "DEngine/Core/Timestep.h"
#include "DEngine/Scene/Scene.h"

namespace DEngine
{
	class System
	{
	public:
		virtual ~System() = default;

		virtual void Start() = 0;
		virtual void OnUpdate(const Timestep& ts, const Scene* scene) = 0;
		virtual void OnRender(const Timestep& ts, const Scene* scene) = 0;
		virtual void Shutdown() = 0;
		
	protected:
		System() = default;
	};
}
