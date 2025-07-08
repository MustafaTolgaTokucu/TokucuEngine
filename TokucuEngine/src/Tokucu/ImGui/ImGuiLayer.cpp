#include "tkcpch.h"
#include "ImGuiLayer.h"

#include "Tokucu/Application.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_vulkan.h"
#include "Tokucu/Renderer/RenderCommand.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"

#include "GLFW/glfw3.h"

namespace Tokucu {



	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{
	}

	ImGuiLayer::~ImGuiLayer()
	{
	}

	void ImGuiLayer::OnAttach()
	{
		// Note: We'll delay ImGui context creation until after Vulkan is initialized
		// This will be done in a separate function called from Application::Run()
		TKC_CORE_INFO("ImGuiLayer attached - context will be created after Vulkan initialization");
	}

	void ImGuiLayer::InitializeImGui()
	{
		if (m_Initialized)
			return;
		m_Initialized = true;
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		m_Context = ImGui::CreateContext();
		if (m_Context == nullptr) {
			TKC_CORE_ERROR("Failed to create ImGui context!");
			return;
		}
		ImGui::SetCurrentContext(m_Context);
		if (ImGui::GetCurrentContext() != m_Context) {
			TKC_CORE_ERROR("Failed to set ImGui context after creation!");
			return;
		}
		TKC_CORE_INFO("ImGui context created and set successfully");
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		// Note: Docking and Viewports require additional ImGui modules
		//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;       // Enable Docking (requires imgui_docking.h)
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;     // Enable Multi-Viewport (requires imgui_viewport.h)
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

		//float fontSize = 18.0f;// *2.0f;
		//io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Bold.ttf", fontSize);
		//io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Regular.ttf", fontSize);

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		// Note: Viewports require additional ImGui modules
		// if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		// {
		//     style.WindowRounding = 0.0f;
		//     style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		// }

		//SetDarkThemeColors();

		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

		// Setup Platform bindings
		ImGui_ImplGlfw_InitForVulkan(window, true);
		
		// Initialize Vulkan backend through the renderer
		RendererAPI* renderer = RenderCommand::GetRendererAPI();
		if (renderer) {
			// Cast to VulkanRendererAPI and call the initialization method
			VulkanRendererAPI* vulkanRenderer = dynamic_cast<VulkanRendererAPI*>(renderer);
			if (vulkanRenderer) {
				vulkanRenderer->InitializeImGuiForVulkan();
				TKC_CORE_INFO("ImGui Vulkan backend initialized successfully");
			} else {
				TKC_CORE_ERROR("Failed to cast renderer to VulkanRendererAPI!");
			}
		} else {
			TKC_CORE_ERROR("No renderer available for ImGui initialization!");
		}
		
		TKC_CORE_INFO("ImGui initialization completed successfully");
	}

	void ImGuiLayer::OnDetach()
	{
		// Note: ImGui_ImplVulkan_Shutdown() should be called from VulkanRendererAPI::Clear()
		// to ensure proper cleanup order
		ImGui_ImplGlfw_Shutdown();
		if (m_Context) {
			ImGui::SetCurrentContext(m_Context);
			ImGui::DestroyContext();
			m_Context = nullptr;
		}
	}




	void ImGuiLayer::Begin()
	{
		// Only proceed if ImGui context is created and set as current
		if (m_Context != nullptr) {
			ImGui::SetCurrentContext(m_Context);
			
			// Verify context is set correctly
			if (ImGui::GetCurrentContext() != m_Context) {
				TKC_CORE_ERROR("Failed to set ImGui context!");
				return;
			}
			
			ImGui_ImplVulkan_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			//ImGuizmo::BeginFrame();
		} else {
			TKC_CORE_ERROR("ImGui context is null in Begin()!");
		}
	}

	void ImGuiLayer::End()
	{
		// Only proceed if ImGui context is created and set as current
		if (m_Context != nullptr) {
			ImGui::SetCurrentContext(m_Context);
			ImGuiIO& io = ImGui::GetIO();
			Application& app = Application::Get();
			io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

			// Rendering
			ImGui::Render();
			// Note: ImGui_ImplVulkan_RenderDrawData will be called from VulkanRendererAPI
			// with the appropriate command buffer

			// For Vulkan, we don't need the OpenGL-style context switching
			// Multi-viewport support for Vulkan is handled differently
			// Note: Viewports require additional ImGui modules
			//if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			//{
			//    ImGui::UpdatePlatformWindows();
			//    ImGui::RenderPlatformWindowsDefault();
			//}
		}
	}

	void ImGuiLayer::OnImGuiRender()
	{
		// Only proceed if ImGui context is created and set as current
		if (m_Context != nullptr) {
			ImGui::SetCurrentContext(m_Context);
			
			// Verify context is set correctly
			if (ImGui::GetCurrentContext() != m_Context) {
				TKC_CORE_ERROR("Failed to set ImGui context in OnImGuiRender()!");
				return;
			}
			
			static bool show = true;
			ImGui::ShowDemoWindow(&show);
			
			// Simple test window
			ImGui::Begin("Tokucu Engine Debug");
			ImGui::Text("Vulkan ImGui Integration Test");
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		} else {
			TKC_CORE_ERROR("ImGui context is null in OnImGuiRender()!");
		}
	}



	

	

}
