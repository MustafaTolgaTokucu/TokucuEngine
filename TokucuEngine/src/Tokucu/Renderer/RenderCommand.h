#pragma once

#include "RendererAPI.h"

namespace Tokucu {

	class RenderCommand
	{
		
	public:

		inline static void initAPI(const std::shared_ptr<Window>& window)
		{
			s_RendererAPI->Init(window);
		}
		inline static void Render()
		{
			s_RendererAPI->Render();
		}

		inline static void SetClearColor(const glm::vec4& color) 
		{
			s_RendererAPI->SetClearColor(color);
		}
		inline static void Clear() 
		{
			s_RendererAPI->Clear();
		}

		inline static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray)
		{
			s_RendererAPI->DrawIndexed(vertexArray);
		}

		inline static void Resize(const std::shared_ptr<Window>& window)
		{
			s_RendererAPI->Resize(window);
		}
	private:
		static RendererAPI* s_RendererAPI;



	};



}
