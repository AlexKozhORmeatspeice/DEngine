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
		~Entity();

		template<typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.try_get<T>(m_Handle);
		}

		template<typename T>
		bool HasComponent() const
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
		T& GetComponent() const
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

		operator bool() const { return m_Handle != entt::null; } 
		bool operator==(const Entity& other) const { return m_Handle == other.m_Handle && m_Scene == other.m_Scene; } 
		bool operator!=(const Entity& other) const { return m_Handle != other.m_Handle || m_Scene != other.m_Scene; } 
		operator uint32_t() const { return (uint32_t)m_Handle; }

	private:
		entt::entity m_Handle{ entt::null };
		Scene* m_Scene;
	};
}
