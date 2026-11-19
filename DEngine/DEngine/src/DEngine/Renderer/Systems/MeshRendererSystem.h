#pragma once

#include "DEngine/Scene/System.h"
#include "DEngine/Renderer/CommandBuffer.h"

#include "vector"

namespace DEngine
{
	class MeshRendererSystem : public System
	{
	public:
		virtual void Start() override {}
		virtual void OnUpdate(const Timestep& ts, Scene* scene) override {}
		virtual void OnRender(const Timestep& ts, Scene* scene) override;
		virtual void Shutdown() override {}

	private:
		Ref<CommandBuffer> GetThreadBuffer(uint32_t frame, uint32_t pass, uint32_t thread);

		uint32_t m_Frame = 0xFFFFFFFFu;
		uint32_t m_Pass = 0;
		std::vector<std::vector<std::vector<Ref<CommandBuffer>>>> m_Buffers;
	};
}
