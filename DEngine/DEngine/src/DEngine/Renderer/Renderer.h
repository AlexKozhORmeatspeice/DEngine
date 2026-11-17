#pragma once

#include "DEngine/Core.h"

#include "RenderCommand.h"
#include "DEngine/Renderer/Cameras/Camera.h"
#include "DEngine/Renderer/Shader/Shader.h"
#include "DEngine/Renderer/Mesh/Mesh.h"
#include "DEngine/Renderer/Material/Material.h"
#include "DEngine/Renderer/Pipeline/RendererPipeline.h"
#include "DEngine/Renderer/Pipeline/PipelineLayout.h"
#include "DEngine/Renderer/Pipeline/DescriptorSet.h"
#include "DEngine/Renderer/Pipeline/RenderPass.h"
#include "DEngine/Renderer/Buffers/UniformBuffer.h"
#include "DEngine/Renderer/Buffers/Framebuffer.h"
#include "DEngine/Renderer/CommandBuffer.h"

#include "glm/glm.hpp"
#include "vector"

enum RenderMode {
	SOLID, WIREFRAME
};

namespace DEngine
{
	class D_API Renderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginFrame();
		static void EndFrame();
		static void BeginScene(const DEngine::Ref<Camera>& camera, const Ref<Framebuffer>& framebuffer = nullptr);
		static void Submit(const Ref<Mesh>& mesh, const Ref<Material>& mat, const glm::mat4& trans, RenderMode mode = RenderMode::SOLID);
		static void EndScene();
		static void SetDirectionalLight(const glm::vec3& direction, const glm::vec3& color);

		static void BeginSwapchainPass();
		static void EndSwapchainPass();

		static void OnWindowResize(uint32_t width, uint32_t height);
		static void ResetIdleCommandBuffers();

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
		static CommandBuffer& GetCommandBuffer();
		static const Ref<PipelineLayout>& GetPipelineLayout() { return m_PipelineLayout; }
		static uint32_t GetFrameInFlightIndex();
		static const Ref<DescriptorSet>& GetFrameSet();
	private:
		struct FrameSlot
		{
			Ref<UniformBuffer> camera;
			Ref<UniformBuffer> light;
			Ref<DescriptorSet> set;
		};

		static void CreatePipelineLayout();
		static void CreatePipeline();
		static void CreateCommandBuffers();
		static void CreateFrameResources();
		static FrameSlot& EnsureFrameSlot(uint32_t image, uint32_t slot);
		static void WriteFrameDescriptors(const Ref<Camera>& camera);
		static RenderPassState MakePassState(uint32_t width, uint32_t height);

		static Ref<RendererPipeline> m_Pipeline;
		static Ref<PipelineLayout> m_PipelineLayout;
		static std::vector<Ref<CommandBuffer>> m_PrimaryCommandBuffers;
		static std::vector<std::vector<FrameSlot>> m_FrameSlots;
		static Ref<RenderPass> m_ActivePass;
		static Ref<Framebuffer> m_ActiveFramebuffer;
		static uint32_t m_CurrentPrimaryCommandBufferInd;
		static uint32_t m_ActiveFrameSlot;
		static uint32_t m_NextFrameSlot;
		static bool m_FrameOpen;
		static bool m_ScenePassOpen;
		static bool m_SwapchainPassRecorded;

		struct DirectionalLight
		{
			glm::vec4 direction{ 0.0f, -1.0f, 0.0f, 0.0f };
			glm::vec4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
			glm::vec4 viewPosition{ 0.0f };
		};

		struct SceneData
		{
			glm::mat4 viewProjMat{ 1.0f };
			Ref<Camera> cam;
			DirectionalLight light;
		};

		static SceneData* s_SceneData;
	};

}
