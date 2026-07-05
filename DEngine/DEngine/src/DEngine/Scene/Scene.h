#pragma once

#include "entt.hpp"
#include "DEngine/Core/Timestep.h"

namespace DEngine
{
	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		Entity& CreateEntity(const std::string& name = std::string());

		//TODO: TEMP
		entt::registry& Reg() { return m_Registry; }

		void OnUpdate(Timestep ts);
	private:
		entt::registry m_Registry;

		friend class Entity;
	};
}
