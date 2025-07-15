#include "Tokucu.h" 
#include "Tokucu/ImGui/SceneEditor.h"
#include "Tokucu/Renderer/RenderCommand.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"


class ExampleLayer : public Tokucu::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{
		//auto cam = camera(100, { 100,100 });
	}

	void OnAttach() override
	{
		// Layer attachment logic can go here if needed
	}

	void OnUpdate() override
	{
		//TKC_INFO("ExampleLayer::Update");
	}

	void OnEvent(Tokucu::Event& event) override
	{
		TKC_TRACE(event.ToString());
	}
};

class Sandbox : public Tokucu::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
		
		// Create and add the SceneEditor layer
		m_SceneEditor = new Tokucu::SceneEditor();
		PushLayer(m_SceneEditor);
		
	}

	~Sandbox()
	{
		// Cleanup is handled by the Application destructor
	}

	void OnAttach() override
	{
	}


private:
	Tokucu::SceneEditor* m_SceneEditor = nullptr;
};

Tokucu::Application* Tokucu::CreateApplication()
{
	return new Sandbox();
}