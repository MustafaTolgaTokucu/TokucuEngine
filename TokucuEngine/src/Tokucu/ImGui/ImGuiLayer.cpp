#include "tkcpch.h"
#include "ImGuiLayer.h"

#include "Tokucu/Application.h"
#include "Tokucu/Renderer/RenderCommand.h"

#include "Platform/Vulkan/VulkanRendererAPI.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <cstring>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <unordered_map>
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

		// -------------- Hierarchy & Inspector Panels --------------
		{
			RendererAPI* rendererAPI = RenderCommand::GetRendererAPI();
			VulkanRendererAPI* vkRenderer = dynamic_cast<VulkanRendererAPI*>(rendererAPI);
			if (vkRenderer) {
				static int selectedIndex = -1;
				static char texPathBuf[256] = "";
				static std::string tempTexType;
				std::vector<VulkanObject>& objs = vkRenderer->GetObjects();

				// Hierarchy window (docked by default)
				ImGui::Begin("Hierarchy");
				// Build grouping by object name so identical model instances appear as children
				std::unordered_map<std::string, std::vector<int>> grouped;
				for (int i = 0; i < (int)objs.size(); ++i) {
					grouped[objs[i].name].push_back(i);
				}

				for (const auto& entry : grouped) {
					const std::string& modelName = entry.first;
					const std::vector<int>& indices = entry.second;

					if (indices.size() == 1) {
						int idx = indices[0];
						ImGui::PushID(idx);
						bool sel = (idx == selectedIndex);
						if (ImGui::Selectable(modelName.c_str(), sel)) {
							selectedIndex = idx;
						}
						ImGui::PopID();
					} else {
						// Parent node
						bool open = ImGui::TreeNode(modelName.c_str());
						if (open) {
							for (int idx : indices) {
								ImGui::PushID(idx);
								std::string label = "Part " + std::to_string(idx);
								bool sel = (idx == selectedIndex);
								if (ImGui::Selectable(label.c_str(), sel)) {
									selectedIndex = idx;
								}
								ImGui::PopID();
							}
							ImGui::TreePop();
						}
					}
				}
				ImGui::End();

				// Inspector window
				if (selectedIndex >= 0 && selectedIndex < objs.size()) {
					VulkanObject& selObj = objs[selectedIndex];
					ImGui::Begin("Inspector");

						// Transform editing (position / rotation / scale)
						auto& transformsMap = vkRenderer->GetObjectTransformations();
						glm::mat4& mat = transformsMap[selObj.name];

						// Decompose matrix
						glm::vec3 translation(mat[3][0], mat[3][1], mat[3][2]);
						glm::vec3 scaleVec(
							glm::length(glm::vec3(mat[0])),
							glm::length(glm::vec3(mat[1])),
							glm::length(glm::vec3(mat[2])));

						glm::mat3 rotMat;
						if (scaleVec.x != 0) rotMat[0] = glm::vec3(mat[0]) / scaleVec.x;
						if (scaleVec.y != 0) rotMat[1] = glm::vec3(mat[1]) / scaleVec.y;
						if (scaleVec.z != 0) rotMat[2] = glm::vec3(mat[2]) / scaleVec.z;

						glm::quat rotQuat = glm::quat_cast(rotMat);
						glm::vec3 rotEuler = glm::degrees(glm::eulerAngles(rotQuat));

						float posArr[3]   = { translation.x, translation.y, translation.z };
						float rotArr[3]   = { rotEuler.x, rotEuler.y, rotEuler.z };
						float scaleArr[3] = { scaleVec.x, scaleVec.y, scaleVec.z };

						bool changed = false;
						changed |= ImGui::DragFloat3("Position", posArr, 0.1f);
						changed |= ImGui::DragFloat3("Rotation", rotArr, 0.5f);
						changed |= ImGui::DragFloat3("Scale",    scaleArr, 0.1f);

						if (changed) {
							translation = glm::vec3(posArr[0], posArr[1], posArr[2]);
							rotEuler = glm::vec3(rotArr[0], rotArr[1], rotArr[2]);
							scaleVec = glm::vec3(scaleArr[0], scaleArr[1], scaleArr[2]);

							glm::quat newQuat = glm::quat(glm::radians(rotEuler));
							glm::mat4 newMat = glm::translate(glm::mat4(1.0f), translation) *
								glm::toMat4(newQuat) *
								glm::scale(glm::mat4(1.0f), scaleVec);

							mat = newMat;
							vkRenderer->MarkObjectAsModified(selObj.name);
						}

					ImGui::Separator();
					ImGui::Text("Textures:");
					for (size_t ti = 0; ti < selObj.textureLocations.size(); ++ti) {
						const auto& [ttype, tpath] = selObj.textureLocations[ti];
						ImGui::Text("%s", ttype.c_str());
						ImGui::SameLine(150);
						ImGui::TextWrapped("%s", tpath.c_str());
						ImGui::SameLine();
						if (ImGui::SmallButton(("Change##" + std::to_string(ti)).c_str())) {
							#ifdef _MSC_VER
								strcpy_s(texPathBuf, sizeof(texPathBuf), tpath.c_str());
							#else
								std::strncpy(texPathBuf, tpath.c_str(), sizeof(texPathBuf));
								texPathBuf[sizeof(texPathBuf) - 1] = '\0';
							#endif
							tempTexType = ttype;
						}
					}

					if (strlen(texPathBuf) > 0) {
						ImGui::InputText("##NewTexPath", texPathBuf, sizeof(texPathBuf));
						ImGui::SameLine();
						if (ImGui::Button("Load Texture")) {
							vkRenderer->UpdateObjectTexture(selObj.name, tempTexType, std::string(texPathBuf));
							memset(texPathBuf, 0, sizeof(texPathBuf));
						}
					}

					ImGui::End();
				}
			}
		}


		ImGui::End(); // DockSpace_Main
	}
}
