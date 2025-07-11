#pragma once
#include "Tokucu/Layer.h"

#include "Tokucu/Events/KeyEvent.h"
#include "Tokucu/Events/MouseEvent.h"
#include "Tokucu/Events/ApplicationEvent.h"
#include "imgui_impl_glfw.h"


namespace Tokucu {

	class TOKUCU_API ImGuiLayer : public Layer 
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		// Initialize ImGui after Vulkan is ready
		void InitializeImGui();

		void Begin();
		void End();
	
	private:
		float m_Time = 0.0f;
		ImGuiContext* m_Context = nullptr;
		bool m_Initialized = false;
	};

}

