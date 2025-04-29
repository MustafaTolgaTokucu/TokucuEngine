#pragma once
#include "Core.h"

#include "Tokucu/Window.h"
#include "Tokucu/LayerStack.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"
#include "Tokucu/ImGui/ImGuiLayer.h"


namespace Tokucu {
	class TOKUCU_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline static Application& Get() { return *s_Instance; }
		inline Window& GetWindow() { return *m_Window; }

	private:
		bool OnWindowClosed(WindowCloseEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);
		
		/////////////////////////////////////////////////////////////////////
		///APP VARIABLES
		/////////////////////////////////////////////////////////////////////
		std::shared_ptr<Window> m_Window;
		
		ImGuiLayer* m_ImGuiLayer;

		bool m_Running = true;
		LayerStack m_LayerStack;

		static Application* s_Instance;
		
	};
	Application* CreateApplication();
}
