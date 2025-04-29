#pragma once

#include "RenderCommand.h"
//#include "GLAD/glad.h"
//#include "Tokucu/Renderer/RendererAPI.h"

namespace Tokucu{



	class Renderer {
	public:

		static void BeginScene(const std::shared_ptr<Window>& window);
		static void EndScene();
		static void RenderScene();
		static void Resize(const std::shared_ptr<Window>& window);
		static void Submit(const std::shared_ptr<VertexArray>& vertexArray);

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

	};


}

