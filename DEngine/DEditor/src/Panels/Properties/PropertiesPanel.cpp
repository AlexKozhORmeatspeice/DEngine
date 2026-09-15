#include "PropertiesPanel.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"



namespace DEngine
{
	Entity PropetiesPanel::m_SelectedEntity = {};

	struct ComponentInfo
	{
		const char* name;
		std::function<bool(Entity&)> has;
		std::function<void(Entity&)> add;
		std::function<void(Entity&)> remove;
		std::function<void(Entity&)> draw;
	};

	static std::vector<ComponentInfo>& GetComponentList()
	{
		static std::vector<ComponentInfo> list = {
			{
				"Rigidbody",
				[](Entity& e) { return e.HasComponent<RigidbodyComponent>(); },
				[](Entity& e) { e.AddComponent<RigidbodyComponent>(); }
			},
			{
				"Collider",
				[](Entity& e) { return e.HasComponent<ColliderComponent>(); },
				[](Entity& e) { e.AddComponent<ColliderComponent>(); }
			},
			// сюда просто добавляем новые компоненты
			// {
			//     "MeshRenderer",
			//     [](Entity& e) { return e.HasComponent<MeshRendererComponent>(); },
			//     [](Entity& e) { e.AddComponent<MeshRendererComponent>(); }
			// },
		};
		return list;
	}

	void PropetiesPanel::OnImGuiRender()
	{
		ImGui::Begin("Properties");

		if (m_SelectedEntity)
		{
			DrawComponents(m_SelectedEntity);
		}

		ImGui::End();
	}

	void PropetiesPanel::SetContext(const Ref<Scene>& scene)
	{
		m_Context = scene;
	}

	void PropetiesPanel::DrawComponents(Entity& entity)
	{
		DrawTag(entity);
		DrawTransform(entity);
		DrawRigidbody(entity);
		DrawCollider(entity);
		DrawAdditionalData(entity);
	}

	void PropetiesPanel::DrawTag(const Entity& entity)
	{
		if (!entity.HasComponent<TagComponent>()) return;

		TagComponent tagComp = entity.GetComponent<TagComponent>();
		
		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		strcpy_s(buffer, sizeof(buffer), tagComp.Tag.c_str());

		if (ImGui::InputText("Tag", buffer, sizeof(buffer)))
		{
			tagComp.Tag = std::string(buffer);
		}
	}
	
	void PropetiesPanel::DrawTransform(Entity& entity)
	{
		if (!entity.HasComponent<TransformComponent>()) return;
		if (!ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform")) return;

		TransformComponent& transComp = entity.GetComponent<TransformComponent>();

		// Position
		glm::vec3 position = transComp.GetPosition();
		if (ImGui::DragFloat3("Position", glm::value_ptr(position), 0.1f))
		{
			transComp.SetPosition(position);
		}

		// Rotation
		glm::vec3 euler = transComp.GetRotationEuler();
		if (ImGui::DragFloat3("Rotation", glm::value_ptr(euler), 0.1f))
		{
			for (int i = 0; i < 3; i++)
			{
				euler[i] = fmod(euler[i], 360.0f);
				if (euler[i] < 0.0f)
					euler[i] += 360.0f;
			}
			transComp.SetRotationEuler(euler);
		}

		// Scale
		glm::vec3 scale = transComp.GetScale();
		if (ImGui::DragFloat3("Scale", glm::value_ptr(scale), 0.1f))
		{
			transComp.SetScale(scale);
		}

		ImGui::TreePop();
	}

	void PropetiesPanel::DrawAdditionalData(Entity& entity)
	{
		if (ImGui::Button("Add component"))
		{
			ImGui::OpenPopup("AddComponent");
		}

		if (ImGui::BeginPopup("AddComponent"))
		{
			static char searchBuffer[128] = "";
			ImGui::InputText("##SearchComponent", searchBuffer, sizeof(searchBuffer));
			ImGui::Separator();

			std::string filter = searchBuffer;
			// можно сделать lower-case, если хочешь поиск без учёта регистра

			for (auto& info : GetComponentList())
			{
				// уже есть — не показываем
				if (info.has(entity))
					continue;

				// фильтр по имени
				if (!filter.empty())
				{
					std::string name = info.name;
					if (name.find(filter) == std::string::npos)
						continue;
				}

				if (ImGui::MenuItem(info.name))
				{
					info.add(entity);
					searchBuffer[0] = '\0'; // очистить поиск
					ImGui::CloseCurrentPopup();
				}
			}

			ImGui::EndPopup();
		}
	}

	void PropetiesPanel::DrawRigidbody(Entity& entity)
	{
		if (!entity.HasComponent<RigidbodyComponent>()) return;
		if (!ImGui::TreeNodeEx((void*)typeid(RigidbodyComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Rigidbody")) return;

		RigidbodyComponent& rb = entity.GetComponent<RigidbodyComponent>();

		// Mass
		float mass = rb.mass;
		if (ImGui::DragFloat("Mass", &mass, 0.1f, 0.01f, 1000.0f))
		{
			rb.mass = mass;
		}

		// Velocity
		glm::vec3 velocity = rb.velocity;
		if (ImGui::DragFloat3("Velocity", glm::value_ptr(velocity), 0.1f))
		{
			rb.velocity = velocity;
		}

		// Acceleration
		glm::vec3 acceleration = rb.acceleration;
		if (ImGui::DragFloat3("Acceleration", glm::value_ptr(acceleration), 0.1f))
		{
			rb.acceleration = acceleration;
		}

		// Force
		glm::vec3 force = rb.force;
		if (ImGui::DragFloat3("Force", glm::value_ptr(force), 0.1f))
		{
			rb.force = force;
		}

		// Use Gravity
		bool useGravity = rb.useGravity;
		if (ImGui::Checkbox("Use Gravity", &useGravity))
		{
			rb.useGravity = useGravity;
		}

		// Is Kinematic
		bool isKinematic = rb.isKinematic;
		if (ImGui::Checkbox("Is Kinematic", &isKinematic))
		{
			rb.isKinematic = isKinematic;
		}

		ImGui::TreePop();
	}

	// В PropertiesPanel.cpp:
	void PropetiesPanel::DrawCollider(Entity& entity)
	{
		if (!entity.HasComponent<ColliderComponent>()) return;
		if (!ImGui::TreeNodeEx((void*)typeid(ColliderComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Collider")) return;

		ColliderComponent& collider = entity.GetComponent<ColliderComponent>();

		// Collider Type
		const char* colliderTypes[] = { "None", "Box", "Sphere", "Capsule", "Mesh" };
		int currentType = static_cast<int>(collider.type);
		if (ImGui::Combo("Type", &currentType, colliderTypes, IM_ARRAYSIZE(colliderTypes)))
		{
			collider.type = static_cast<ColliderType>(currentType);
		}

		// Size (для Box)
		if (collider.type == ColliderType::Box)
		{
			glm::vec3 size = collider.size;
			if (ImGui::DragFloat3("Size", glm::value_ptr(size), 0.1f, 0.01f, 100.0f))
			{
				collider.size = size;
			}
		}

		// Radius (для Sphere и Capsule)
		if (collider.type == ColliderType::Sphere || collider.type == ColliderType::Capsule)
		{
			float radius = collider.radius;
			if (ImGui::DragFloat("Radius", &radius, 0.1f, 0.01f, 100.0f))
			{
				collider.radius = radius;
			}
		}

		// Height (для Capsule)
		if (collider.type == ColliderType::Capsule)
		{
			float height = collider.height;
			if (ImGui::DragFloat("Height", &height, 0.1f, 0.01f, 100.0f))
			{
				collider.height = height;
			}
		}

		// Offset (для всех типов)
		glm::vec3 offset = collider.offset;
		if (ImGui::DragFloat3("Offset", glm::value_ptr(offset), 0.1f))
		{
			collider.offset = offset;
		}

		// Is Trigger
		bool isTrigger = collider.isTrigger;
		if (ImGui::Checkbox("Is Trigger", &isTrigger))
		{
			collider.isTrigger = isTrigger;
		}

		ImGui::TreePop();
	}
}