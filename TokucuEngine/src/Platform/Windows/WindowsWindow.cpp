#include "tkcpch.h"
#include "WindowsWindow.h"

#include "Tokucu/Events/ApplicationEvent.h"
#include "Tokucu/Events/KeyEvent.h"
#include "Tokucu/Events/MouseEvent.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "GLFW/glfw3.h"

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
		ImGuiIO& io = ImGui::GetIO();
		auto camera = Camera::GetInstance();
		
		// Always allow keyboard input when cursor is disabled (free-look mode)
		if (!m_CursorEnabled) {
			// In free-look mode, allow all keyboard input regardless of ImGui state
			if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				WindowCloseEvent event;
				data.EventCallback(event);
			}
			
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
				camera->ProcessKeyboard(FORWARD);
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
				camera->ProcessKeyboard(BACKWARD);
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
				camera->ProcessKeyboard(LEFT);
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
				camera->ProcessKeyboard(RIGHT);
			
			static bool pKeyWasPressed = false;
			if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
				if (!pKeyWasPressed) {
					m_CursorEnabled = !m_CursorEnabled;
					glfwSetInputMode(m_Window, GLFW_CURSOR, m_CursorEnabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
					// Reset firstMouse when toggling cursor to prevent camera jumps
					firstMouse = true;
					pKeyWasPressed = true;
				}
			}
			else {
				pKeyWasPressed = false;
			}
			return;
		}
		
		// Normal mode - check if keyboard is captured by ImGui
		if (io.WantCaptureKeyboard) {
			// Check if we're over the viewport window specifically
			ImGuiContext* context = ImGui::GetCurrentContext();
			ImGuiWindow* hoveredWindow = context ? context->HoveredWindow : nullptr;
			if (hoveredWindow && strstr(hoveredWindow->Name, "Viewport") != nullptr) {
				// Allow keyboard input for viewport window
				if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
				{
					WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
					WindowCloseEvent event;
					data.EventCallback(event);
				}
				
				if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
					camera->ProcessKeyboard(FORWARD);
				if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
					camera->ProcessKeyboard(BACKWARD);
				if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
					camera->ProcessKeyboard(LEFT);
				if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
					camera->ProcessKeyboard(RIGHT);
				
				static bool pKeyWasPressed = false;
				if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
					if (!pKeyWasPressed) {
						m_CursorEnabled = !m_CursorEnabled;
						glfwSetInputMode(m_Window, GLFW_CURSOR, m_CursorEnabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
						// Reset firstMouse when toggling cursor to prevent camera jumps
						firstMouse = true;
						pKeyWasPressed = true;
					}
				}
				else {
					pKeyWasPressed = false;
				}
			}
			else {
				return; // Block keyboard input if ImGui wants keyboard and we're not over viewport
			}
		}
	}

	void WindowsWindow::mouse_callback(double xpos, double ypos)
	{
		// Always allow mouse input when cursor is disabled (free-look mode)
		if (!m_CursorEnabled) {
			auto camera = Camera::GetInstance();
			
			if (firstMouse)
			{
				lastX = xpos;
				lastY = ypos;
				yaw = -90;
				pitch = 0;
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

			camera->updateCameraVectors(yaw, pitch);
			return;
		}
		
		// Normal mode - check ImGui mouse capture
		ImGuiIO& io = ImGui::GetIO();
		if (io.WantCaptureMouse) {
			// Check if we're over the viewport window specifically
			ImGuiContext* context = ImGui::GetCurrentContext();
			ImGuiWindow* hoveredWindow = context ? context->HoveredWindow : nullptr;
			if (hoveredWindow && strstr(hoveredWindow->Name, "Viewport") != nullptr)
			{
				// Allow camera input for viewport window
				auto camera = Camera::GetInstance();
				
				if (firstMouse)
				{
					lastX = xpos;
					lastY = ypos;
					yaw = -90;
					pitch = 0;
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

				camera->updateCameraVectors(yaw, pitch);
			}
			else {
				return; // Block camera input if ImGui wants mouse and we're not over viewport
			}
		}
		else {
			// If ImGui doesn't want mouse capture, allow camera movement regardless
			auto camera = Camera::GetInstance();

			if (firstMouse)
			{
				lastX = xpos;
				lastY = ypos;
				yaw = -90;
				pitch = 0;
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

			camera->updateCameraVectors(yaw, pitch);
		}
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
		glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
		glfwWindowHint(GLFW_FLOATING, GLFW_FALSE);
		glfwWindowHint(GLFW_MAXIMIZED, GLFW_FALSE);

		m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
		//test for imgui
		glfwMaximizeWindow(m_Window);
		
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

		glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				// Handle framebuffer resize for Vulkan rendering
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				// The framebuffer size callback is called when the framebuffer is resized
				// This is important for Vulkan rendering
			});

		glfwSetWindowPosCallback(m_Window, [](GLFWwindow* window, int xpos, int ypos)
			{
				// Handle window position changes for multi-viewport support
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				// This callback is called when the main window moves
				// ImGui multi-viewport should handle this automatically
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				WindowCloseEvent event;
				data.EventCallback(event);
			});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				// Get the window instance to check cursor state
				WindowsWindow* instance = (WindowsWindow*)(glfwGetWindowUserPointer(window));
				
				// Always allow keyboard events when cursor is disabled (free-look mode)
				if (instance && !instance->m_CursorEnabled) {
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
					return;
				}
				
				// Normal mode - check ImGui keyboard capture
				ImGuiIO& io = ImGui::GetIO();
				if (io.WantCaptureKeyboard) {
					// Check if we're over the viewport window specifically
					ImGuiContext* context = ImGui::GetCurrentContext();
					ImGuiWindow* hoveredWindow = context ? context->HoveredWindow : nullptr;
					if (hoveredWindow && strstr(hoveredWindow->Name, "Viewport") != nullptr) {
						// Allow keyboard input for viewport window
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
					}
					return; // Block engine event if ImGui wants keyboard and we're not over viewport
				}
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
				// Get the window instance to check cursor state
				WindowsWindow* instance = (WindowsWindow*)(glfwGetWindowUserPointer(window));
				
				// Always allow mouse button events when cursor is disabled (free-look mode)
				if (instance && !instance->m_CursorEnabled) {
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
					return;
				}
				
				// Normal mode - check ImGui mouse capture
				ImGuiIO& io = ImGui::GetIO();
				if (io.WantCaptureMouse) {
					// Check if we're over the viewport window specifically
					ImGuiContext* context = ImGui::GetCurrentContext();
					ImGuiWindow* hoveredWindow = context ? context->HoveredWindow : nullptr;
					if (hoveredWindow && strstr(hoveredWindow->Name, "Viewport") != nullptr) {
						// Allow input for viewport window
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
					}
					return; // Block engine event if ImGui wants mouse and we're not over viewport
				}
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
				if (ImGui::IsAnyItemActive() || ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
					return; // Only block if ImGui is hovered or an item is active
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				MouseScrolledEvent event((float)xOffset, (float)yOffset);
				data.EventCallback(event);
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
			{
				// Get the window instance to check cursor state
				WindowsWindow* instance = (WindowsWindow*)(glfwGetWindowUserPointer(window));
				
				// Always allow mouse movement when cursor is disabled (free-look mode)
				if (instance && !instance->m_CursorEnabled) {
					WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
					MouseMovedEvent event((float)xPos, (float)yPos);
					data.EventCallback(event);
					instance->mouse_callback(xPos, yPos);
					return;
				}
				
				// Normal mode - check ImGui mouse capture
				ImGuiIO& io = ImGui::GetIO();
				if (io.WantCaptureMouse) {
					// Check if we're over the viewport window specifically
					ImGuiContext* context = ImGui::GetCurrentContext();
					ImGuiWindow* hoveredWindow = context ? context->HoveredWindow : nullptr;
					if (hoveredWindow && strstr(hoveredWindow->Name, "Viewport") != nullptr) {
						// Allow input for viewport window
						WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
						MouseMovedEvent event((float)xPos, (float)yPos);
						data.EventCallback(event);
						instance->mouse_callback(xPos, yPos);
					}
					return; // Block engine event if ImGui wants mouse and we're not over viewport
				}
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
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
			
		}

	void WindowsWindow::SetVSync(bool enabled)
	{
		m_Data.VSync = enabled;
	}

	bool WindowsWindow::IsVSync() const
	{
		return m_Data.VSync;
	}
}