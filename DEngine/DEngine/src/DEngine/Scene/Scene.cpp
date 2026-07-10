#include "dpch.h"
#include "Scene.h"

#include "Entity.h"
#include "Components.h"

#include "DEngine/Scene/System.h"
#include "DEngine/Scene/Systems.h"

namespace DEngine
{
	Scene::Scene()
	{
		entt::entity entity = m_Registry.create();

		//Список всех систем подгружаемых в обработчики
		//При реализации новой системы ее стоят добавлять только сюда в соответствии с примерами ниже
		//После добавления автоматически будет работать
		AddSystem(std::make_shared<MeshRendererSystem>());
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

	void Scene::AddSystem(std::shared_ptr<System> system)
	{
		for (const auto& sys : m_Systems)
		{
			if (typeid(*sys) == typeid(*system))
			{
				D_CORE_WARN("System is already setted to scene");
				return;
			}
		}

		m_Systems.push_back(system);
	}

	void Scene::OnUpdate(const Timestep& ts)
	{
		for (auto system : m_Systems)
		{
			system->OnUpdate(ts, const_cast<const Scene*>(this));
		}
	}

	void Scene::OnRender(const Timestep& ts)
	{
		for (auto system : m_Systems)
		{
			system->OnRender(ts, const_cast<const Scene*>(this));
		}
	}
}