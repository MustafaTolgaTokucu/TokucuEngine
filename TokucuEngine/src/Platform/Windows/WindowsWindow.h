#pragma once
#include "Tokucu/Window.h"
#include "Tokucu/Renderer/GraphicsContext.h"
#include "GLFW/glfw3.h"

#include "Tokucu/Renderer/Camera.h"


namespace Tokucu {

	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void OnUpdate() override;

		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }

		// Window attributes
		inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

		virtual void* GetNativeWindow() const { return m_Window; }
		
		void processInput(GLFWwindow* window);
		void mouse_callback(double xpos, double ypos);


	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
		
	private:
		GLFWwindow*  m_Window;
		GraphicsContext* m_Context;

		bool firstMouse = true;
		float lastX;
		float lastY;
		float yaw;
		float pitch;

		double previousTime;
		int frameCount;

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
		
		
	};

}
