#pragma once

#include "Scene.h"
#include "entt.hpp"

namespace DEngine
{
	class Entity
	{
	public:
		Entity();
		Entity(entt::entity _handle, Scene* scene);
		Entity(const Entity& other) = default;

		template<typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.try_get<T>(m_Handle);
		}

		template<typename T, typename ... Args>
		T& AddComponent(Args&&... args)
		{
			D_CORE_ASSERT(!HasComponent<T>(), "Entity already has component");

			return m_Scene->m_Registry.emplace<T>(m_Handle, std::forward<Args>(args)...);
		}

		template<typename T>
		T& GetComponent()
		{
			D_CORE_ASSERT(HasComponent<T>(), "Entity doesn't have component");

			return m_Scene->m_Registry.get<T>(m_Handle);
		}

		template<typename T>
		void RemoveComponent()
		{
			D_CORE_ASSERT(HasComponent<T>(), "Entity doesn't have component to remove");

			return m_Scene->m_Registry.remove<T>(m_Handle);
		}

		//TODO: я немного бля убиться хочу от этого, но нормик думаю. Если не впдалу - сделайте лучше
		operator bool() const { return *((int*)&m_Handle) != 0; } 

	private:
		entt::entity m_Handle{ entt::null };
		Scene* m_Scene;
	};
}
