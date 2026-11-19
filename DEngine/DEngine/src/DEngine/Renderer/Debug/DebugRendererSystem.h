#pragma once

#include "DEngine/Scene/System.h"
#include "DEngine/Renderer/CommandBuffer.h"

#include "vector"

namespace DEngine
{
	class DebugRendererSystem : public System
	{
	public:
		DebugRendererSystem() {}
		virtual ~DebugRendererSystem() {}

		virtual void Start() override {}
		virtual void OnUpdate(const Timestep& ts, Scene* scene) override {}
		virtual void OnRender(const Timestep& ts, Scene* scene) override;
		virtual void Shutdown() override {}

	private:
		Ref<CommandBuffer> GetPassBuffer();

		uint32_t m_Frame = 0xFFFFFFFFu;
		uint32_t m_Pass = 0;
		std::vector<std::vector<Ref<CommandBuffer>>> m_Buffers;
	};
}
