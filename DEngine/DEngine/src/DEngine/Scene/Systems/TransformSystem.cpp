#include "dpch.h"
#include "TransformSystem.h"

#include "DEngine/Scene/Entity.h"
#include "DEngine/Scene/Components.h"
#include "DEngine/Profiler/Profiler.h"

#include <vector>

namespace DEngine
{
	void TransformSystem::OnUpdate(const Timestep& ts, Scene* scene)
	{
		D_PROFILE_FUNCTION();
		UpdateWorld(scene);
	}

	void TransformSystem::UpdateWorld(Scene* scene)
	{
		D_PROFILE_FUNCTION();

		auto& registry = scene->GetRegistry();
		auto view = scene->View<TransformComponent>();

		m_VisitState.clear();
		m_VisitState.reserve(view.size());

		m_Stack.clear();
		m_Stack.reserve(64);

		for (auto [entity, transform] : view.each())
		{
			(void)transform;
			ComposeIterative(scene, entity, registry);
		}
	}

	void TransformSystem::ComposeIterative(Scene* scene, entt::entity root, entt::registry& registry)
	{
		// Если корень уже посчитан — выходим сразу
		auto rootIt = m_VisitState.find(root);
		if (rootIt != m_VisitState.end() && rootIt->second == VisitState::Done)
			return;

		m_Stack.push_back(root);

		while (!m_Stack.empty())
		{
			entt::entity handle = m_Stack.back();

			auto stateIt = m_VisitState.find(handle);
			VisitState state = (stateIt != m_VisitState.end()) ? stateIt->second : VisitState::Unvisited;

			if (state == VisitState::Done)
			{
				m_Stack.pop_back();
				continue;
			}

			TransformComponent& transform = registry.get<TransformComponent>(handle);

			// Определяем родителя
			entt::entity parentHandle = entt::null;
			if (transform.Parent != 0)
			{
				Entity parent = scene->FindEntity(transform.Parent);
				if (parent && parent.HasComponent<TransformComponent>())
					parentHandle = parent.GetHandle();
			}

			// Если родитель ещё не посчитан — сначала обработаем его
			if (parentHandle != entt::null)
			{
				auto parentIt = m_VisitState.find(parentHandle);
				VisitState parentState = (parentIt != m_VisitState.end()) ? parentIt->second : VisitState::Unvisited;

				if (parentState == VisitState::Unvisited)
				{
					m_VisitState[handle] = VisitState::Visiting;
					m_VisitState[parentHandle] = VisitState::Visiting;
					m_Stack.push_back(parentHandle);
					continue;
				}
				else if (parentState == VisitState::Visiting)
				{
					transform.world = transform.trans;
					m_VisitState[handle] = VisitState::Done;
					m_Stack.pop_back();
					D_CORE_ASSERT(false, "Cycle detected in transform hierarchy");
					continue;
				}
			}

			glm::mat4 local = transform.trans;
			if (parentHandle != entt::null)
			{
				const TransformComponent& parentTransform = registry.get<TransformComponent>(parentHandle);
				transform.world = parentTransform.world * local;
			}
			else
			{
				transform.world = local;
			}

			m_VisitState[handle] = VisitState::Done;
			m_Stack.pop_back();
		}
	}
}