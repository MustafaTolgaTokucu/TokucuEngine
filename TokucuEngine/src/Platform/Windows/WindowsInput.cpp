#include "tkcpch.h"
#include "WindowsInput.h"
#include "GLFW/glfw3.h"
#include "Tokucu/Application.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

namespace Tokucu {


	Input* Input::s_Instance = new WindowsInput();

	bool WindowsInput::IsKeyPressedImpl(int keycode)
	{
		ImGuiIO& io = ImGui::GetIO();
		if (io.WantCaptureKeyboard)
			return false; // Block engine input if ImGui wants keyboard
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state= glfwGetKey(window, keycode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool WindowsInput::IsMouseButtonPressedImpl(int button)
	{
		ImGuiIO& io = ImGui::GetIO();
		// Check if mouse is over any ImGui window that wants input
		if (io.WantCaptureMouse) {
			// Check if we're over the viewport window specifically
			ImGuiContext* context = ImGui::GetCurrentContext();
			ImGuiWindow* hoveredWindow = context ? context->HoveredWindow : nullptr;
			if (hoveredWindow && strstr(hoveredWindow->Name, "Viewport") != nullptr) {
				// Allow input for viewport window
				auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
				auto state = glfwGetMouseButton(window, button);
				return state == GLFW_PRESS;
			}
			return false; // Block engine input if ImGui wants mouse and we're not over viewport
		}
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}

	std::pair<float, float> WindowsInput::GetMousePositionImpl()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		return { (float)xpos, (float)ypos };
	}

	float WindowsInput::GetMOuseXImpl()
	{

		auto [x, y] = GetMousePositionImpl();
		return x;
	}

	

	float WindowsInput::GetMouseYImpl()
	{
		auto [x, y] = GetMousePositionImpl();
		return y;
	}

}