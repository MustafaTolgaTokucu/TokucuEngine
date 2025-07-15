#include "tkcpch.h"
#include "ImGuiLayer.h"

#include "Tokucu/Application.h"
#include "Tokucu/Renderer/RenderCommand.h"

#include "Platform/Vulkan/VulkanRendererAPI.h"
#include "SceneEditor.h"

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
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;       // Enable Docking

		
		// Disable cursor changes to prevent cursor info from showing in viewport
		io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

		//float fontSize = 18.0f;// *2.0f;
		//io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Bold.ttf", fontSize);
		//io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Regular.ttf", fontSize);

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();



		//SetDarkThemeColors();

		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

		// Setup Platform bindings
		ImGui_ImplGlfw_InitForVulkan(window, true);
		
		// Initialize Vulkan backend through the renderer
		RendererAPI* renderer = RenderCommand::GetRendererAPI();
		VulkanRendererAPI* vulkanRenderer = dynamic_cast<VulkanRendererAPI*>(renderer);
		vulkanRenderer->initImGui();
		TKC_CORE_INFO("ImGui Vulkan backend initialized successfully");
		
		TKC_CORE_INFO("ImGui initialization completed successfully");
	}

	void ImGuiLayer::OnDetach()
	{
		ImGui_ImplGlfw_Shutdown();
		if (m_Context) {
			ImGui::SetCurrentContext(m_Context);
			ImGui::DestroyContext();
			m_Context = nullptr;
		}
	}

	void ImGuiLayer::Begin()
	{
		ImGui::SetCurrentContext(m_Context);
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::End()
	{
		ImGui::SetCurrentContext(m_Context);
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

		// Rendering
		ImGui::Render();
	}

	void ImGuiLayer::OnImGuiRender()
	{
		ImGui::SetCurrentContext(m_Context);

			// Find SceneEditor layer
			SceneEditor* sceneEditor = nullptr;
			auto& layerStack = Application::Get().m_LayerStack;
			for (auto it = layerStack.begin(); it != layerStack.end(); ++it) {
				sceneEditor = dynamic_cast<SceneEditor*>(*it);
				if (sceneEditor) break;
			}
			if (!sceneEditor) {
				ImGui::Text("SceneEditor layer not found!");
				return;
			}

			// Panel visibility state (static so it persists)
			static bool showViewport = true;

			// Main DockSpace that fills the entire window
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			Application& app = Application::Get();
			ImVec2 windowSize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());
			
			// Ensure valid window size
			if (windowSize.x <= 0) windowSize.x = 1280.0f;
			if (windowSize.y <= 0) windowSize.y = 720.0f;
			
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			//ImGui::SetNextWindowViewport(viewport->ID);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			ImGui::Begin("DockSpace_Main", nullptr, window_flags);
			ImGui::PopStyleVar(2);
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

			// Menu bar for reopening closed windows
			if (ImGui::BeginMenuBar()) {
				if (ImGui::BeginMenu("Window")) {
					ImGui::MenuItem("Viewport", nullptr, &showViewport);
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}

			// Create the viewport that fills the entire dockspace
			if (showViewport) {
				// Force the viewport to fill the entire dockspace
				ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);
				ImGui::Begin("Viewport", &showViewport,
					ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNavFocus);
				// Set the flag to allow SceneEditor to render
				sceneEditor->SetRenderFromImGuiLayer(true);
				sceneEditor->OnImGuiRender();
				sceneEditor->SetRenderFromImGuiLayer(false);
				ImGui::End();
			}



			ImGui::End(); // DockSpace_Main
	}
}
