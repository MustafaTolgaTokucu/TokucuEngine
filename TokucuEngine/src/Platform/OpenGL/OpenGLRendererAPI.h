#pragma once

#include "Tokucu/Renderer/RendererAPI.h"

namespace Tokucu {

	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void Init(const std::shared_ptr<Window> & window) override;

		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;
		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) override;
		virtual void Render() override;
	};

}
