#pragma once
#include "Tokucu/Renderer/VertexArray.h"
#include "Tokucu/Window.h"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>

namespace Tokucu {

	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0, OpenGL = 1, Vulkan=2
		};
	public:
		//virtual ~RendererAPI() = default;

		virtual void Init(const std::shared_ptr<Window>& window) = 0;
		//virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;

		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) = 0;
		//virtual void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) = 0;
		virtual void Render() = 0;
		virtual void Resize(const std::shared_ptr<Window>& window) = 0;
		//virtual void SetLineWidth(float width) = 0;

		inline static API GetAPI() { return s_API; }
		//static Scope<RendererAPI> Create();
	private:
		static API s_API;
	};

}

