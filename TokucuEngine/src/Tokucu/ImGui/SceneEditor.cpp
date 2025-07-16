#include "tkcpch.h"
#include "SceneEditor.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_vulkan.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <algorithm>
#include "Platform/Vulkan/VulkanRendererAPI.h"
#include <cstring>
#include "Tokucu/Renderer/Camera.h"
#include "Tokucu/Renderer/RenderCommand.h"

//#define ImTextureID ImU64
namespace Tokucu {

	SceneEditor::SceneEditor()
		: Layer("SceneEditor")
	{
	}

	void SceneEditor::OnAttach()
	{
		TKC_CORE_INFO("SceneEditor attached");
	}

	void SceneEditor::OnDetach()
	{
		TKC_CORE_INFO("SceneEditor detached");
	}

	void SceneEditor::OnImGuiRender()
	{
		// Only render if called from ImGuiLayer, not from the layer stack directly
		if (!m_RenderFromImGuiLayer) {
			return;
		}
		
		// Get the current window size
		ImVec2 windowSize = ImGui::GetContentRegionAvail();
		
		// Ensure minimum size
		if (windowSize.x < 100.0f) windowSize.x = 100.0f;
		if (windowSize.y < 100.0f) windowSize.y = 100.0f;
		
		// Get the texture ID from the renderer
		RendererAPI* renderer = RenderCommand::GetRendererAPI();
		VulkanRendererAPI* vulkanRenderer = dynamic_cast<VulkanRendererAPI*>(renderer);
		
		// Get the ImGui texture ID
		ImTextureID textureId = vulkanRenderer->GetImGuiTextureId();
		
		// Calculate UV coordinates for the texture
		ImVec2 uv0(0.0f, 0.0f);
		ImVec2 uv1(1.0f, 1.0f);
		
		// Render the texture in the ImGui window
		ImGui::Image(textureId, windowSize, uv0, uv1);
	}


	void SceneEditor::InitializeViewportTexture()
	{
		// --- BEGIN: Use Offscreen Resolve Image for ImGui Viewport ---
		if (m_ViewportTextureInitialized)
		{
			TKC_CORE_INFO("Viewport texture already initialized, skipping");
			return;
		}

		TKC_CORE_INFO("Initializing viewport texture...");

		// Get the Vulkan renderer
		auto renderer = static_cast<VulkanRendererAPI*>(RenderCommand::GetRendererAPI());
		if (!renderer)
		{
			TKC_CORE_ERROR("Failed to get Vulkan renderer for viewport texture initialization");
			return;
		}

		// Offscreen resources are now created during Vulkan initialization, no need to create them here

		// Use the ImGui texture view that's properly set up by the renderer
		VkImageView imGuiTextureView = renderer->GetImGuiTextureView();
		VkSampler offscreenSampler = renderer->GetTextureSampler(); // Use your main sampler or create a dedicated one
		
		// Add safety checks
		if (imGuiTextureView == VK_NULL_HANDLE)
		{
			TKC_CORE_ERROR("ImGui texture view is null - cannot initialize viewport texture");
			return;
		}
		
		if (offscreenSampler == VK_NULL_HANDLE)
		{
			TKC_CORE_ERROR("Texture sampler is null - cannot initialize viewport texture");
			return;
		}

		TKC_CORE_INFO("Got texture view and sampler, registering with ImGui...");

		// Register the ImGui texture with ImGui
		m_ViewportTexture = (ImU64)ImGui_ImplVulkan_AddTexture(offscreenSampler, imGuiTextureView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		if (m_ViewportTexture)
		{
			m_ViewportTextureInitialized = true;
			TKC_CORE_INFO("Viewport texture initialized successfully: {}", (void*)m_ViewportTexture);
		}
		else
		{
			TKC_CORE_ERROR("Failed to initialize viewport texture (offscreen resolve)");
		}
		// --- END: Use Offscreen Resolve Image for ImGui Viewport ---
	}


} 