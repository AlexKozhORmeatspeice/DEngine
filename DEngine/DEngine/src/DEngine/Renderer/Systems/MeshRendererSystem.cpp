#include "dpch.h"
#include "MeshRendererSystem.h"

#include "DEngine/Scene/Components.h"
#include "DEngine/Scene/Entity.h"
#include "DEngine/Renderer/Renderer.h"

#include "DEngine/Asset/AssetManager.h"
#include "DEngine/Profiler/Profiler.h"

#include "DEngine/Jobs/JobSystem.h"

#include "algorithm"

namespace DEngine
{
	Ref<CommandBuffer> MeshRendererSystem::GetThreadBuffer(uint32_t frame, uint32_t pass, uint32_t thread)
	{
		if (m_Buffers.size() <= frame)
			m_Buffers.resize(frame + 1);
		if (m_Buffers[frame].size() <= pass)
			m_Buffers[frame].resize(pass + 1);
		if (m_Buffers[frame][pass].size() <= thread)
			m_Buffers[frame][pass].resize(thread + 1);

		Ref<CommandBuffer>& buffer = m_Buffers[frame][pass][thread];
		if (!buffer)
		{
			const Ref<CommandPool>& pool = CommandPoolSystem::Get(thread);
			if (pool)
				buffer = pool->AllocateSecondary(false);
		}

		return buffer;
	}

	void MeshRendererSystem::OnRender(const Timestep& ts, Scene* scene)
	{
		D_PROFILE_FUNCTION();
		(void)ts;

		auto lightComps = scene->View<DirectLightComponent, TransformComponent>();
		auto comps = scene->View<TagComponent, MeshRendererComponent, TransformComponent>();
		const auto* entityStorage = comps.handle();

		for (auto [entity, light, lightTrans] : lightComps.each())
		{
			if (!scene->IsActive(Entity(entity, scene)))
				continue;

			Renderer::SetDirectionalLight(lightTrans.GetForward(), light.color);
			break;
		}

		const uint32_t count = entityStorage ? static_cast<uint32_t>(entityStorage->size()) : 0;
		if (count == 0)
			return;

		if (Renderer::GetAPI() != RendererAPI::API::Vulkan)
		{
			for (uint32_t i = 0; i < count; ++i)
			{
				const auto entity = (*entityStorage)[i];
				if (!comps.contains(entity))
					continue;

				const auto& tag = comps.get<TagComponent>(entity);
				const auto& meshComp = comps.get<MeshRendererComponent>(entity);
				const auto& trans = comps.get<TransformComponent>(entity);
				if (!tag.activeSelf || trans.unactiveByParent)
					continue;

				Ref<Material> mat = AssetManager::GetAsset<Material>(meshComp.materialHandle);
				Ref<Mesh> mesh = AssetManager::GetAsset<Mesh>(meshComp.meshHandle);
				if (!mat || !mesh)
					continue;

				Renderer::Submit(Renderer::GetCommandBuffer(), mesh, mat, trans.GetModelMatrix());
			}
			return;
		}

		const uint32_t frame = Renderer::GetFrameInFlightIndex();
		if (m_Frame != frame)
		{
			m_Frame = frame;
			m_Pass = 0;
		}

		const uint32_t pass = m_Pass++;
		const uint32_t threadCount = JobSystem::ThreadCount();
		if (threadCount == 0)
			return;

		const uint32_t chunk = (count + threadCount - 1) / threadCount;
		std::vector<Ref<CommandBuffer>> recorded;
		std::vector<PinnedJob> jobs;
		recorded.reserve(threadCount);
		jobs.reserve(threadCount);

		for (uint32_t thread = 0; thread < threadCount; ++thread)
		{
			const uint32_t start = thread * chunk;
			const uint32_t end = std::min(count, start + chunk);
			if (start >= end)
				continue;

			Ref<CommandBuffer> buffer = GetThreadBuffer(frame, pass, thread);
			if (!buffer)
			{
				D_CORE_ERROR("Got null buffer");
				continue;
			}

			recorded.push_back(buffer);
			jobs.emplace_back(thread, [buffer, start, end, &comps, entityStorage]()
			{
				CommandBufferInheritance inheritance{};
				inheritance.framebuffer = Renderer::GetCurrentFramebuffer().get();
				inheritance.renderPass = Renderer::GetActivePass().get();

				buffer->Reset();
				buffer->BeginSecondary(inheritance);
				Renderer::BindSceneState(*buffer);

				for (uint32_t i = start; i < end; ++i)
				{
					const auto entity = (*entityStorage)[i];
					if (!comps.contains(entity))
						continue;

					const auto& tag = comps.get<TagComponent>(entity);
					const auto& meshComp = comps.get<MeshRendererComponent>(entity);
					const auto& trans = comps.get<TransformComponent>(entity);
					if (!tag.activeSelf || trans.unactiveByParent)
						continue;

					Ref<Material> mat = AssetManager::GetAsset<Material>(meshComp.materialHandle);
					Ref<Mesh> mesh = AssetManager::GetAsset<Mesh>(meshComp.meshHandle);
					if (!mat || !mesh)
						continue;

					Renderer::Submit(*buffer, mesh, mat, trans.GetModelMatrix());
				}

				buffer->End();
			});
		}

		for (PinnedJob& job : jobs)
			JobSystem::Schedule(job);
		for (PinnedJob& job : jobs)
			JobSystem::Wait(job);

		Renderer::GetCommandBuffer().ExecuteCommands(recorded);
	}
}
