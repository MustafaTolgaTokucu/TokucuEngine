
#include "tkcpch.h"
#include "Application.h"
#include "Tokucu/Log.h"
#include "Tokucu/Input.h"
#include "Tokucu/Renderer/Renderer.h"


namespace Tokucu {
#define BIND_EVENT_FN(x) std::bind(&Application::x,this,std::placeholders::_1)
	Application* Application::s_Instance = nullptr;

	Tokucu::Application::Application()
	{
		TKC_CORE_ASSERT("Application already exists");
		s_Instance = this;
		m_Window = std::shared_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
		
		// Create ImGui layer but don't attach it yet
		m_ImGuiLayer = new ImGuiLayer();
	}

	Tokucu::Application::~Application()
	{
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClosed));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResized));
		/*for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			(*--it)->OnEvent(e);
			if (e.m_Handled)
			{
				break;
			}
		}*/
	}
	/////////////////////////////////////////////////
	////Run Application
	/////////////////////////////////////////////////
	void Application::Run()
	{
		// Initialize renderer first
		Renderer::BeginScene(m_Window);
		
		// Initialize ImGui after renderer is ready
		m_ImGuiLayer->InitializeImGui();
		
		// Now attach ImGui layer after renderer is initialized
		PushOverlay(m_ImGuiLayer);
		
		while (m_Running)
		{
			Renderer::RenderScene();

			for (Layer* layer : m_LayerStack)
				layer->OnUpdate();

			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
				layer->OnImGuiRender();
			m_ImGuiLayer->End();

			/*auto [x, y] = Input::GetMousePosition();
			TKC_CORE_TRACE("{0} , {1}", x, y);*/

			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClosed(WindowCloseEvent& e)
	{
		TKC_CORE_ERROR("Exiting");
		m_Running = false;
		Renderer::EndScene();
		return true;
	}

	bool Application::OnWindowResized(WindowResizeEvent& e)
	{
		Renderer::Resize(m_Window);
		return true;
	}

}