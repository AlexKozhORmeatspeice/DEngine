#include "dpch.h"
#include "CameraSystem.h"

#include "DEngine/Scene/Entity.h"

#include "DEngine/Scene/Components.h"
#include "DEngine/Renderer/Cameras/PerspCamera.h"
#include "DEngine/Renderer/Cameras/OrthCamera.h"
#include "DEngine/Profiler/Profiler.h"

namespace DEngine
{
	void CameraSystem::SetViewport(uint32_t width, uint32_t height)
	{
		m_Width = width;
		m_Height = height;
	}

	void CameraSystem::OnUpdate(const Timestep& ts, Scene* scene)
	{
		D_PROFILE_FUNCTION();
		(void)ts;

		bool found = false;
		bool foundPrimary = false;
		CameraComponent settings;
		glm::vec3 position{ 0.0f };
		glm::quat rotation{ 1.0f, 0.0f, 0.0f, 0.0f };
		auto cameras = scene->View<CameraComponent, TransformComponent>();

		for (auto [entity, camera, transform] : cameras.each())
		{
			if (foundPrimary)
				continue;
			if (!scene->IsActive(Entity(entity, scene)))
				continue;

			if (!found || camera.primary)
			{
				settings = camera;
				position = transform.GetWorldPosition();
				rotation = transform.GetWorldRotation();
				found = true;
				foundPrimary = camera.primary;
			}
		}

		if (!found)
		{
			m_SceneCamera = nullptr;
			m_HasCamera = false;
			return;
		}

		m_SceneCamera = EnsureCamera(settings.projectionType);
		ApplySettings(settings);
		m_SceneCamera->SetPos(position);
		m_SceneCamera->SetOrientation(rotation);
		m_HasCamera = true;
	}

	Ref<Camera> CameraSystem::EnsureCamera(CameraProjectionType type)
	{
		if (m_HasCamera && m_SceneCamera && m_ActiveType == type)
			return m_SceneCamera;

		m_ActiveType = type;
		uint32_t width = m_Width > 0 ? m_Width : 1280;
		uint32_t height = m_Height > 0 ? m_Height : 720;

		if (type == CameraProjectionType::Orthographic)
			return CreateRef<OrthographicCamera>(10.0f, (float)width / (float)height);

		return CreateRef<PerspectiveCamera>(60.0f, width, height);
	}

	void CameraSystem::ApplySettings(const CameraComponent& camera)
	{
		if (m_Width > 0 && m_Height > 0)
			m_SceneCamera->ChangeSize(m_Width, m_Height);

		float nearClip = camera.nearClip;
		float farClip = camera.farClip;
		if (nearClip < 0.001f)
			nearClip = 0.001f;
		if (farClip <= nearClip)
			farClip = nearClip + 0.01f;

		if (camera.projectionType == CameraProjectionType::Perspective)
		{
			auto& perspective = static_cast<PerspectiveCamera&>(*m_SceneCamera);
			perspective.SetVerticalFov(camera.fov);
			perspective.SetClipPlanes(nearClip, farClip);
		}
		else
		{
			auto& orthographic = static_cast<OrthographicCamera&>(*m_SceneCamera);
			orthographic.SetSize(camera.orthoSize);
			orthographic.SetClipPlanes(nearClip, farClip);
		}
	}
}
