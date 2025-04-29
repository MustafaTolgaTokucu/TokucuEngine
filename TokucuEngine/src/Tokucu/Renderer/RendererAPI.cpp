#include "tkcpch.h"
#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"

namespace Tokucu {

	RendererAPI::API RendererAPI::s_API = RendererAPI::API::Vulkan;
	
	/*Scope<RendererAPI> RendererAPI::Create()
	{
		switch (s_API)
		{
		case RendererAPI::API::None:    TKC_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateScope<OpenGLRendererAPI>();
		}

		TKC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}*/

}