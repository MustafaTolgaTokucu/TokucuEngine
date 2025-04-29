#include "tkcpch.h"
#include "Renderer.h"

namespace Tokucu {
	void Renderer::BeginScene(const std::shared_ptr<Window>& window)
	{
		RenderCommand::initAPI(window);
	}
	void Renderer::RenderScene()
	{
		RenderCommand::Render();
	}
	void Renderer::Resize(const std::shared_ptr<Window>& window)
	{
		RenderCommand::Resize(window);
	}
	void Renderer::EndScene()
	{
		RenderCommand::Clear();
	}
	
	void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray)
	{
		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
}