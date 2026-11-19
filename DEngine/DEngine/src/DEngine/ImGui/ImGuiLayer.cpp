#include "dpch.h"
#include "ImGuiLayer.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "DEngine/Application.h"
#include "DEngine/Profiler/Profiler.h"
#include "DEngine/Renderer/RendererAPI.h"
#include "DEngine/Renderer/Renderer.h"
#include "DEngine/Platform/Vulkan/VulkanImGuiHelper.h"

//TODO: TEMP
#include "GLFW/glfw3.h"
#include "glad/glad.h"

namespace DEngine
{
	ImGuiLayer::ImGuiLayer()
		: Layer("ImGUI_layer")
	{

	}

	ImGuiLayer::~ImGuiLayer()
	{

	}

	void ImGuiLayer::Init()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		if (RendererAPI::GetAPI() != RendererAPI::API::Vulkan)
			io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

		if (RendererAPI::GetAPI() == RendererAPI::API::Vulkan)
		{
			ImGui_ImplGlfw_InitForVulkan(window, true);
			VulkanImGuiHelper::Init();
		}
		else
		{
			ImGui_ImplGlfw_InitForOpenGL(window, true);
			ImGui_ImplOpenGL3_Init("#version 410");
		}
	}

	void ImGuiLayer::Shutdown()
	{
		if (RendererAPI::GetAPI() == RendererAPI::API::Vulkan)
			VulkanImGuiHelper::Shutdown();
		else
			ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnImGuiRenderer()
	{
	}

	void ImGuiLayer::OnEvent(Event& event)
	{
		if (m_BlockEvents)
		{
			ImGuiIO& io = ImGui::GetIO();
			event.m_Handled |= event.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
			event.m_Handled |= event.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
		}
	}

	void ImGuiLayer::Begin()
	{
		D_PROFILE_FUNCTION();
		if (RendererAPI::GetAPI() == RendererAPI::API::OpenGL)
			ImGui_ImplOpenGL3_NewFrame();
		else if (RendererAPI::GetAPI() == RendererAPI::API::Vulkan)
			VulkanImGuiHelper::NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::OnResize(uint32_t width, uint32_t height)
	{
		if (RendererAPI::GetAPI() == RendererAPI::API::Vulkan)
			VulkanImGuiHelper::OnResize(Application::Get().GetSwapChain().GetImageCount());
	}

	void ImGuiLayer::End()
	{
		D_PROFILE_FUNCTION();
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());

		ImGui::Render();
		if (RendererAPI::GetAPI() == RendererAPI::API::Vulkan)
		{
			if (!Renderer::IsFrameOpen())
				return;

			Renderer::BeginSwapchainPass();
			VulkanImGuiHelper::Render();
			Renderer::EndSwapchainPass();
			return;
		}

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_cont = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_cont);
		}
	}
}
