#pragma once

#include "DEngine/Scene/System.h"
#include "DEngine/Renderer/Cameras/Camera.h"
#include "DEngine/Scene/Components/CameraComponent.h"

namespace DEngine
{
	class CameraSystem : public System
	{
	public:
		CameraSystem() = default;
		virtual ~CameraSystem() = default;

		virtual void Start() override {}
		virtual void OnUpdate(const Timestep& ts, Scene* scene) override;
		virtual void OnRender(const Timestep& ts, Scene* scene) override {}
		virtual void Shutdown() override {}

		void SetViewport(uint32_t width, uint32_t height);

		Ref<Camera> GetSceneCamera() const { return m_SceneCamera; }
		bool HasSceneCamera() const { return m_SceneCamera != nullptr; }

	private:
		Ref<Camera> EnsureCamera(CameraProjectionType type);
		void ApplySettings(const CameraComponent& camera);

		Ref<Camera> m_SceneCamera;
		CameraProjectionType m_ActiveType = CameraProjectionType::Perspective;
		bool m_HasCamera = false;
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
	};
}
