#include "dpch.h"
#include "Renderer.h"

#include "DEngine/Application.h"
#include "DEngine/Asset/AssetManager.h"
#include "DEngine/Renderer/Device/RenderDevice.h"
#include "DEngine/Renderer/Pipeline/RenderPass.h"
#include "DEngine/SwapChain/SwapChain.h"

namespace DEngine
{
	Renderer::SceneData* Renderer::s_SceneData = new Renderer::SceneData;
	Ref<PipelineLayout> Renderer::m_PipelineLayout;
	Ref<RendererPipeline> Renderer::m_Pipeline;
	std::vector<Ref<CommandBuffer>> Renderer::m_PrimaryCommandBuffers;
	std::vector<std::vector<Renderer::FrameSlot>> Renderer::m_FrameSlots;
	Ref<RenderPass> Renderer::m_ActivePass;
	Ref<Framebuffer> Renderer::m_ActiveFramebuffer;
	uint32_t Renderer::m_CurrentPrimaryCommandBufferInd = 0;
	uint32_t Renderer::m_ActiveFrameSlot = 0;
	uint32_t Renderer::m_NextFrameSlot = 0;
	bool Renderer::m_FrameOpen = false;
	bool Renderer::m_ScenePassOpen = false;
	bool Renderer::m_SwapchainPassRecorded = false;

	void Renderer::Init()
	{
		RenderCommand::Init();

		CreatePipelineLayout();
		CreatePipeline();
		CreateCommandBuffers();
		CreateFrameResources();
	}

	void Renderer::Shutdown()
	{
		m_PrimaryCommandBuffers.clear();
		m_FrameSlots.clear();
		m_ActivePass = nullptr;
		m_ActiveFramebuffer = nullptr;
		m_Pipeline = nullptr;
		m_PipelineLayout = nullptr;
		m_FrameOpen = false;
	}

	void Renderer::BeginFrame()
	{
		if (m_PrimaryCommandBuffers.empty())
			return;
		SwapChain& swapChain = Application::Get().GetSwapChain();
		D_CORE_ASSERT(swapChain.AcquireNextImage(m_CurrentPrimaryCommandBufferInd), "Failed to get next swap chain framebuffer");

		CommandBuffer& commandBuffer = *m_PrimaryCommandBuffers[m_CurrentPrimaryCommandBufferInd];
		commandBuffer.Reset();
		commandBuffer.Begin();
		m_NextFrameSlot = 0;
		m_FrameOpen = true;
		m_SwapchainPassRecorded = false;
	}

	void Renderer::EndFrame()
	{
		if (!m_FrameOpen)
			return;

		if (m_ScenePassOpen)
			EndScene();

		if (!m_SwapchainPassRecorded)
		{
			BeginSwapchainPass();
			EndSwapchainPass();
		}

		SwapChain& swapChain = Application::Get().GetSwapChain();
		CommandBuffer& commandBuffer = *m_PrimaryCommandBuffers[m_CurrentPrimaryCommandBufferInd];
		commandBuffer.End();
		D_CORE_ASSERT(swapChain.Submit(commandBuffer, m_CurrentPrimaryCommandBufferInd), "Failed to present swap chain framebuffer");
		m_FrameOpen = false;
	}

	RenderPassState Renderer::MakePassState(uint32_t width, uint32_t height)
	{
		RenderPassState passState;
		passState.width = width;
		passState.height = height;
		DClearValue colorClear;
		colorClear.type = DClearType::Color;
		colorClear.color[0] = 0.1f;
		colorClear.color[1] = 0.1f;
		colorClear.color[2] = 0.1f;
		colorClear.color[3] = 1.0f;
		DClearValue depthClear;
		depthClear.type = DClearType::DepthStencil;
		depthClear.depth = 1.0f;
		depthClear.stencil = 1;
		passState.clearValues.push_back(colorClear);
		passState.clearValues.push_back(depthClear);
		passState.contents = DSubpassContents::Inline;
		return passState;
	}

	uint32_t Renderer::GetFrameInFlightIndex()
	{
		return Application::Get().GetSwapChain().GetCurrentFrame();
	}

	const Ref<DescriptorSet>& Renderer::GetFrameSet()
	{
		static Ref<DescriptorSet> empty;
		if (m_CurrentPrimaryCommandBufferInd >= m_FrameSlots.size())
			return empty;

		const std::vector<FrameSlot>& slots = m_FrameSlots[m_CurrentPrimaryCommandBufferInd];
		if (m_ActiveFrameSlot >= slots.size())
			return empty;

		return slots[m_ActiveFrameSlot].set;
	}

	Renderer::FrameSlot& Renderer::EnsureFrameSlot(uint32_t image, uint32_t slot)
	{
		if (m_FrameSlots.size() <= image)
			m_FrameSlots.resize(image + 1);

		std::vector<FrameSlot>& slots = m_FrameSlots[image];
		while (slots.size() <= slot)
		{
			FrameSlot created;
			created.camera = UniformBuffer::Create(sizeof(glm::mat4));
			created.light = UniformBuffer::Create(sizeof(DirectionalLight));
			created.set = DescriptorSet::Create(m_PipelineLayout, 0);
			created.set->WriteUniform(0, created.camera);
			created.set->WriteUniform(1, created.light);
			slots.push_back(created);
		}

		return slots[slot];
	}

	void Renderer::WriteFrameDescriptors(const Ref<Camera>& camera)
	{
		s_SceneData->viewProjMat = camera->GetViewProjMat();
		s_SceneData->cam = camera;
		s_SceneData->light.viewPosition = glm::vec4(camera->GetPos(), 1.0f);

		FrameSlot& slot = EnsureFrameSlot(m_CurrentPrimaryCommandBufferInd, m_NextFrameSlot);
		slot.camera->SetData(&s_SceneData->viewProjMat, sizeof(glm::mat4));
		slot.light->SetData(&s_SceneData->light, sizeof(DirectionalLight));
		m_ActiveFrameSlot = m_NextFrameSlot;
		m_NextFrameSlot++;
	}

	void Renderer::BeginScene(const DEngine::Ref<Camera>& camera, const Ref<Framebuffer>& framebuffer)
	{
		if (!m_FrameOpen || !camera)
			return;
		if (m_ScenePassOpen)
			EndScene();

		SwapChain& swapChain = Application::Get().GetSwapChain();
		Ref<Framebuffer> target = framebuffer ? framebuffer : swapChain.GetFramebuffer(m_CurrentPrimaryCommandBufferInd);
		const FramebufferSpecification& spec = target->GetSpecification();
		Ref<RenderPass> renderPass = target->GetRenderPass() ? target->GetRenderPass() : swapChain.GetRenderPass();

		WriteFrameDescriptors(camera);

		CommandBuffer& commandBuffer = *m_PrimaryCommandBuffers[m_CurrentPrimaryCommandBufferInd];

		if (RendererAPI::GetAPI() == RendererAPI::API::OpenGL)
			target->Bind(commandBuffer);

		renderPass->Begin(commandBuffer, *target, MakePassState(spec.Width, spec.Height));
		m_ActivePass = renderPass;
		m_ActiveFramebuffer = target;
		m_Pipeline->Bind(commandBuffer);
		m_Pipeline->SetViewport(commandBuffer, 0.0f, 0.0f, static_cast<float>(spec.Width), static_cast<float>(spec.Height));
		m_FrameSlots[m_CurrentPrimaryCommandBufferInd][m_ActiveFrameSlot].set->Bind(commandBuffer, m_PipelineLayout);
		m_ScenePassOpen = true;
	}

	void Renderer::Submit(const Ref<Mesh>& mesh, const Ref<Material>& mat, const glm::mat4& trans, RenderMode mode)
	{
		if (!mesh || !mat || !m_ScenePassOpen)
			return;

		Ref<VertexArray> vertexArray = mesh->GetVertexArray();
		if (!vertexArray || !vertexArray->GetIndexBuffer())
			return;

		CommandBuffer& commandBuffer = *m_PrimaryCommandBuffers[m_CurrentPrimaryCommandBufferInd];
		mat->Bind(commandBuffer);
		m_PipelineLayout->PushConstant(commandBuffer, DShaderStage::Vertex, 0, sizeof(glm::mat4), &trans);
		mesh->Bind(commandBuffer);

		switch (mode)
		{
			case WIREFRAME:
				RenderCommand::DrawIndexedWireframe(commandBuffer, vertexArray);
				break;
			case SOLID:
			default:
				RenderCommand::DrawIndexed(commandBuffer, vertexArray);
				break;
		}
	}

	void Renderer::EndScene()
	{
		if (!m_ScenePassOpen)
			return;

		CommandBuffer& commandBuffer = *m_PrimaryCommandBuffers[m_CurrentPrimaryCommandBufferInd];
		if (m_ActivePass)
			m_ActivePass->End(commandBuffer);
		if (RendererAPI::GetAPI() == RendererAPI::API::OpenGL && m_ActiveFramebuffer)
			m_ActiveFramebuffer->Unbind(commandBuffer);
		m_ActivePass = nullptr;
		m_ActiveFramebuffer = nullptr;
		m_ScenePassOpen = false;
	}

	void Renderer::SetDirectionalLight(const glm::vec3& direction, const glm::vec3& color)
	{
		s_SceneData->light.direction = glm::vec4(direction, 0.0f);
		s_SceneData->light.color = glm::vec4(color, 1.0f);
	}

	void Renderer::BeginSwapchainPass()
	{
		if (!m_FrameOpen || m_SwapchainPassRecorded)
			return;

		if (m_ScenePassOpen)
			EndScene();

		SwapChain& swapChain = Application::Get().GetSwapChain();
		Ref<Framebuffer> target = swapChain.GetFramebuffer(m_CurrentPrimaryCommandBufferInd);
		CommandBuffer& commandBuffer = *m_PrimaryCommandBuffers[m_CurrentPrimaryCommandBufferInd];
		swapChain.GetRenderPass()->Begin(commandBuffer, *target, MakePassState(swapChain.GetWidth(), swapChain.GetHeight()));
		m_SwapchainPassRecorded = true;
	}

	void Renderer::EndSwapchainPass()
	{
		if (!m_SwapchainPassRecorded)
			return;

		SwapChain& swapChain = Application::Get().GetSwapChain();
		CommandBuffer& commandBuffer = *m_PrimaryCommandBuffers[m_CurrentPrimaryCommandBufferInd];
		swapChain.GetRenderPass()->End(commandBuffer);
	}

	CommandBuffer& Renderer::GetCommandBuffer()
	{
		return *m_PrimaryCommandBuffers[m_CurrentPrimaryCommandBufferInd];
	}

	void Renderer::ResetIdleCommandBuffers()
	{
		for (uint32_t i = 0; i < m_PrimaryCommandBuffers.size(); ++i)
		{
			if (m_FrameOpen && i == m_CurrentPrimaryCommandBufferInd)
				continue;

			if (m_PrimaryCommandBuffers[i])
				m_PrimaryCommandBuffers[i]->Reset();
		}
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0)
			return;

		Application::Get().GetSwapChain().Resize(width, height);
		CreateCommandBuffers();
		CreateFrameResources();
		RenderCommand::SetViewport(width, height);
	}

	void Renderer::CreatePipelineLayout()
	{
		PipelineLayoutState layoutState;

		DDescriptorSetLayout frameSet;
		frameSet.bindings.push_back({ 0, DDescriptorType::UniformBuffer, DShaderStage::Vertex, 1 });
		frameSet.bindings.push_back({ 1, DDescriptorType::UniformBuffer, DShaderStage::Fragment, 1 });
		layoutState.setLayouts.push_back(frameSet);

		DDescriptorSetLayout materialSet;
		materialSet.bindings.push_back({ 0, DDescriptorType::CombinedImageSampler, DShaderStage::Fragment, 1 });
		layoutState.setLayouts.push_back(materialSet);

		layoutState.pushConstantRanges.push_back({ DShaderStage::Vertex, 0, sizeof(glm::mat4) });

		m_PipelineLayout = PipelineLayout::Create(Application::Get().GetDevice(), layoutState);
	}

	void Renderer::CreatePipeline()
	{
		const SwapChain& swapChain = Application::Get().GetSwapChain();

		auto pipelineState = RendererPipeline::CreateDefaultPipelineState(swapChain.GetWidth(), swapChain.GetHeight());
		pipelineState.renderPass = swapChain.GetRenderPass();
		pipelineState.pipelineLayout = m_PipelineLayout;

		m_Pipeline = RendererPipeline::Create(
			AssetManager::GetAsset<Shader>(AssetManager::GetBaseRendererShaderHandle()),
			Application::Get().GetDevice(),
			pipelineState);
	}

	void Renderer::CreateCommandBuffers()
	{
		SwapChain& swapChain = Application::Get().GetSwapChain();
		RenderDevice& device = Application::Get().GetDevice();

		CommandBufferState state;
		state.level = DCommandBufferLevel::Primary;

		m_PrimaryCommandBuffers.clear();
		m_PrimaryCommandBuffers.resize(swapChain.GetImageCount());
		for (uint32_t i = 0; i < swapChain.GetImageCount(); i++)
			m_PrimaryCommandBuffers[i] = CommandBuffer::Create(device, state);
	}

	void Renderer::CreateFrameResources()
	{
		const uint32_t imageCount = Application::Get().GetSwapChain().GetImageCount();
		m_FrameSlots.clear();
		m_FrameSlots.resize(imageCount);
		m_ActiveFrameSlot = 0;
		m_NextFrameSlot = 0;

		for (uint32_t image = 0; image < imageCount; image++)
			EnsureFrameSlot(image, 0);
	}
}
