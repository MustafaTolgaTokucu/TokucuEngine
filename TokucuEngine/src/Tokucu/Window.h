#pragma once
#include "tkcpch.h"
#include "Core.h"



#include "Events/Event.h"


namespace Tokucu {

	struct WindowProps
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;

		WindowProps(const std::string& title = "Tokucu Engine",
			unsigned int width = 1920,
			unsigned int height = 1080)
			: Title(title), Width(width), Height(height)
		{

		}

	};
	// Interface Representing a desktop system based Window
	class TOKUCU_API Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;
		virtual ~Window(){}
		virtual void OnUpdate() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		// Window attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;
		static Window* Create(const WindowProps& props = WindowProps());
		

	};


}