#include "tkcpch.h"
#include "WindowsWindow.h"

#include "Platform/OpenGL/OpenGLContext.h"
//#include "GLFW/glfw3.h"
#include "Tokucu/Events/ApplicationEvent.h"
#include "Tokucu/Events/KeyEvent.h"
#include "Tokucu/Events/MouseEvent.h"




namespace Tokucu
{
	
	static bool s_GLFWInitialized = false;
	
	static void GLFWErrorCallback(int error, const char* description)
	{
		TKC_CORE_ERROR("GLFW Error ({0}):{1}", error, description);
	}
	Window* Window::Create(const WindowProps& props)
	{
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		

		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		

		Shutdown();
	}

	void WindowsWindow::processInput(GLFWwindow* window)
	{
		auto camera= Camera::GetInstance();
		//
		//if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) 
		//{	
		//	TKC_CORE_INFO("mode: line");
		//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//}
		//if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
		//{
		//	TKC_CORE_INFO("mode: line");
		//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//}
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.EventCallback(event);
		}
			//Shutdown();
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera->ProcessKeyboard(FORWARD);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera->ProcessKeyboard(BACKWARD);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera->ProcessKeyboard(LEFT);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera->ProcessKeyboard(RIGHT);
		if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		{
			glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}

	void WindowsWindow::mouse_callback(double xpos, double ypos)
	{
		auto camera = Camera::GetInstance();
		

		if (firstMouse )
		{
			lastX = 960;
			lastY = 540;
			yaw = -90;
			pitch = 0;
			TKC_CORE_INFO("first");
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}
		
		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos;
		lastX = xpos;
		lastY = ypos;
		
		float sensitivity = 0.1f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;
		
		
		yaw += xoffset;
		pitch += yoffset;

		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
		TKC_CORE_INFO("yaw :{0} pitch: {1} ", yaw, pitch);

		camera->updateCameraVectors(yaw, pitch);
		
	}

	void WindowsWindow::Init(const WindowProps& props)
	{
		//TODO abstraction for vulkan and opengl

		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		

		TKC_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

		if (!s_GLFWInitialized)
		{

			int success = glfwInit();

			TKC_CORE_ASSERT(success, "Could not initialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
			s_GLFWInitialized = true;
		}


		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
		//m_Context = new OpenGLContext(m_Window);
		//m_Context->Init();
		//glfwSetCursorPosCallback(m_Window, mouse_callback);
		//m_Context = new VulkanContext(m_Window);
		//m_Context->Init();
		
		previousTime = glfwGetTime();
		frameCount = 0;


		
		
		glfwSetWindowUserPointer(m_Window, &m_Data);

		SetVSync(true);
		// Set GLFW callbacks
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.Width = width;
				data.Height = height;

				WindowResizeEvent event(width, height);
				data.EventCallback(event);
				
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				WindowCloseEvent event;
				data.EventCallback(event);
			});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, 0);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, true);
					data.EventCallback(event);
					break;
				}
				}
			});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				KeyTypedEvent event(keycode);
				data.EventCallback(event);
			});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data.EventCallback(event);
					break;
				}
				}
			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseScrolledEvent event((float)xOffset, (float)yOffset);
				data.EventCallback(event);
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				WindowsWindow* instance = (WindowsWindow*)(glfwGetWindowUserPointer(window));
				MouseMovedEvent event((float)xPos, (float)yPos);
				data.EventCallback(event);
				instance->mouse_callback(xPos, yPos);
				
			});
		
	}

	void WindowsWindow::Shutdown()
		{
			

			glfwDestroyWindow(m_Window);
		
		}

		void WindowsWindow::OnUpdate()
		{

			frameCount++;
			double currentTime = glfwGetTime();
			if (currentTime - previousTime >= 1.0) {
				// Calculate FPS and update window title
				char title[256];
				snprintf(title, sizeof(title), "Tokucu Engine - FPS: %d", frameCount);

				glfwSetWindowTitle(m_Window, title);

				frameCount = 0;
				previousTime = currentTime;
			}
			processInput(m_Window);
			glfwPollEvents();
			

			//m_Context->SwapBuffers();
			
		}

		void WindowsWindow::SetVSync(bool enabled)
		{
			//TODO for OPENGL

			//if (enabled)
			//	//glfwSwapInterval(1);
			//else
			//	//glfwSwapInterval(0);
			//
			m_Data.VSync = enabled;
		}

		bool WindowsWindow::IsVSync() const
		{
			return m_Data.VSync;
		}


}