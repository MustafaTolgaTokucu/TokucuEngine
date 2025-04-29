#include "tkcpch.h"
#include "RenderCommand.h"


#include "Platform/OpenGL/OpenGLRendererAPI.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"

namespace Tokucu 
{
	
	//RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
	RendererAPI* RenderCommand::s_RendererAPI = new VulkanRendererAPI;
}