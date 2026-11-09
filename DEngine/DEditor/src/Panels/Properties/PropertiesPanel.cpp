#include "PropertiesPanel.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "DEngine/Asset/EditorAssetManager.h"
#include "Panels/AssetDragDrop.h"



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
			{
				"Mesh Renderer",
				[](Entity& e) { return e.HasComponent<MeshRendererComponent>(); },
				[](Entity& e)
				{
					auto& renderer = e.AddComponent<MeshRendererComponent>();
					renderer.meshHandle = AssetManager::GetPrimitiveMesh(PrimitiveType::Cube);
				}
			},
			{
				"Camera",
				[](Entity& e) { return e.HasComponent<CameraComponent>(); },
				[](Entity& e)
				{
					auto& camera = e.AddComponent<CameraComponent>();
					camera.primary = true;
					if (!e.GetScene())
						return;

					auto cameras = e.GetScene()->View<CameraComponent>();
					for (auto [entity, other] : cameras.each())
					{
						if (entity != e.GetHandle())
						{
							camera.primary = false;
							break;
						}
					}
				}
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


	Entity& PropetiesPanel::getSelectedEntity()
	{
		return m_SelectedEntity;
	}

	void PropetiesPanel::SetContext(const Ref<Scene>& scene)
	{
		m_Context = scene;
		m_SelectedEntity = {};
	}

	void PropetiesPanel::DrawComponents(Entity& entity)
	{
		DrawTag(entity);
		DrawTransform(entity);
		DrawCamera(entity);
		DrawMeshRenderer(entity);
		DrawRigidbody(entity);
		DrawCollider(entity);
		DrawAdditionalData(entity);
	}

	void PropetiesPanel::DrawTag(Entity& entity)
	{
		if (!entity.HasComponent<TagComponent>()) return;

		TagComponent& tagComp = entity.GetComponent<TagComponent>();

		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		strcpy_s(buffer, sizeof(buffer), tagComp.Tag.c_str());

		if (ImGui::InputText("Tag", buffer, sizeof(buffer)))
			tagComp.Tag = buffer;
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

	void PropetiesPanel::DrawAssetSlot(const char* label, AssetHandle& slot, AssetType accepted, bool allowPrimitives)
	{
		ImGui::PushID(label);

		const std::string name = AssetHelper::DisplayName(slot);
		float width = ImGui::GetContentRegionAvail().x;
		if (allowPrimitives)
			width -= 60.0f;
		if (width < 40.0f)
			width = 40.0f;
		ImGui::Button(name.c_str(), ImVec2(width, 0.0f));
		AcceptAssetInto(slot, accepted);

		if (allowPrimitives)
		{
			ImGui::SameLine();
			if (ImGui::Button("Set"))
				ImGui::OpenPopup("PrimitiveMesh");

			if (ImGui::BeginPopup("PrimitiveMesh"))
			{
				if (ImGui::MenuItem("Cube"))
					slot = AssetManager::GetPrimitiveMesh(PrimitiveType::Cube);
				if (ImGui::MenuItem("Sphere"))
					slot = AssetManager::GetPrimitiveMesh(PrimitiveType::Sphere);
				if (ImGui::MenuItem("Clear"))
					slot = AssetHandle(static_cast<uint64_t>(AssetHandle::Invalid()));
				ImGui::EndPopup();
			}
		}

		ImGui::PopID();
	}

	void PropetiesPanel::DrawCamera(Entity& entity)
	{
		if (!entity.HasComponent<CameraComponent>())
			return;
		if (!ImGui::TreeNodeEx((void*)typeid(CameraComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Camera"))
			return;

		CameraComponent& camera = entity.GetComponent<CameraComponent>();

		const char* projectionNames[] = { "Perspective", "Orthographic" };
		int projection = static_cast<int>(camera.projectionType);
		if (ImGui::Combo("Projection", &projection, projectionNames, IM_ARRAYSIZE(projectionNames)))
			camera.projectionType = static_cast<CameraProjectionType>(projection);

		if (camera.projectionType == CameraProjectionType::Perspective)
			ImGui::DragFloat("Field of View", &camera.fov, 0.1f, 1.0f, 179.0f);
		else
			ImGui::DragFloat("Size", &camera.orthoSize, 0.1f, 0.01f, 10000.0f);

		ImGui::DragFloat("Near", &camera.nearClip, 0.01f, 0.001f, 100000.0f);
		ImGui::DragFloat("Far", &camera.farClip, 1.0f, 0.01f, 100000.0f);
		if (camera.nearClip < 0.001f)
			camera.nearClip = 0.001f;
		if (camera.farClip <= camera.nearClip)
			camera.farClip = camera.nearClip + 0.01f;

		bool primary = camera.primary;
		if (ImGui::Checkbox("Primary", &primary))
		{
			camera.primary = primary;
			if (primary && entity.GetScene())
			{
				auto cameras = entity.GetScene()->View<CameraComponent>();
				for (auto [otherEntity, other] : cameras.each())
				{
					if (otherEntity != entity.GetHandle())
						other.primary = false;
				}
			}
		}

		ImGui::TreePop();
	}

	void PropetiesPanel::DrawMeshRenderer(Entity& entity)
	{
		if (!entity.HasComponent<MeshRendererComponent>())
			return;
		if (!ImGui::TreeNodeEx((void*)typeid(MeshRendererComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Mesh Renderer"))
			return;

		MeshRendererComponent& renderer = entity.GetComponent<MeshRendererComponent>();

		ImGui::Text("Mesh");
		DrawAssetSlot("Mesh", renderer.meshHandle, AssetType::Mesh, true);

		ImGui::Text("Material");
		DrawAssetSlot("Material", renderer.materialHandle, AssetType::Material, false);

		ImGui::TreePop();
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