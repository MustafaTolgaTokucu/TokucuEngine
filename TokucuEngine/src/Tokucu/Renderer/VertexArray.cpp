#include "tkcpch.h"
#include "VertexArray.h"

#include "Tokucu/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Tokucu {

	VertexArray* VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    TKC_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return new OpenGLVertexArray();
		case RendererAPI::API::Vulkan:	TKC_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		}

		TKC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}