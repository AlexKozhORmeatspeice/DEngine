#include "dpch.h"
#include "Scene.h"

#include "Entity.h"
#include "Components.h"

namespace DEngine
{
	Scene::Scene()
	{
		entt::entity entity = m_Registry.create();
	}

	Scene::~Scene()
	{
		
	}

	Entity& Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };

		entity.AddComponent<TransformComponent>();

		auto& tag = entity.AddComponent<TagComponent>(name);
		tag.Tag = name.empty() ? "Entity" : name;

		return entity;
	}

	void Scene::OnUpdate(Timestep ts)
	{
		
	}
}