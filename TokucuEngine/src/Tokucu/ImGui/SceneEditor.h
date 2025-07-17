#pragma once
#include "Tokucu/Layer.h"
#include "imgui/imgui.h"
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <unordered_map>

namespace Tokucu {

	class SceneEditor : public Layer
	{
	public:
		SceneEditor();
		~SceneEditor() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		
		void SetRenderFromImGuiLayer(bool renderFromImGuiLayer) { m_RenderFromImGuiLayer = renderFromImGuiLayer; }

	private:
		

		
		// Viewport texture handling
		ImTextureID m_ViewportTexture = 0;
		bool m_ViewportTextureInitialized = false;
		bool m_RenderFromImGuiLayer = false;
		// Selection state for hierarchy
		int m_SelectedObjectIndex = -1;
		char m_TexturePathBuffer[256] = ""; // buffer for texture path input
		std::string m_TempTextureType;
		// Runtime FBX model loading buffers
		char m_ModelPathBuffer[256] = "";
		char m_ModelTextureDirBuffer[256] = "";
		//
		void InitializeViewportTexture();
		ImVec2 m_LastViewportPanelSize = ImVec2(0, 0);
	};

} 