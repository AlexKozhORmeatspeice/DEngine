#pragma once

#include "entt.hpp"
#include "DEngine/Core/Timestep.h"
#include "DEngine/Events/Event.h"

#include "memory"

namespace DEngine
{
	class Entity;
	class System;

	//Класс содержащий набор сущностей, компонент и систем их обработки
	class Scene
	{
	public:
		Scene();
		~Scene();

		Entity& CreateEntity(const std::string& name = std::string());

		template<typename... Components>
		auto View()
		{
			return m_Registry.view<Components...>();
		}

		template<typename... Components>
		auto View() const
		{
			return m_Registry.view<Components...>();
		}

		void OnUpdate(const Timestep& ts);
		void OnRender(const Timestep& ts);

		const std::vector<Entity>& GetAllEntities() const { return m_Entities; }
	private:
		void AddSystem(std::shared_ptr<System> sys);

		std::vector<std::shared_ptr<System>> m_Systems;
		std::vector<Entity> m_Entities;
		entt::registry m_Registry;

		friend class Entity;
		friend class ScenePanel;
		friend class SceneSerializer;
	};
}
