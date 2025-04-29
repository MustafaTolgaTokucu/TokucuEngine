#pragma once
#include "Tokucu/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Tokucu {

	class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(GLFWwindow* windowHandle);

		virtual void Init() override;
		virtual void SwapBuffers() override;
	private:
		GLFWwindow* m_WindowHandle;
		///////////////////////////////
		//////OpenGL Initializer variables//////////
		////////////////////////////////
		// unsigned int fbo;
		//unsigned int frameBufferTexture;
		//
		//unsigned int uboMatrices;
		//unsigned int ssbo;//shader storage buffer object
		//
		//unsigned int texture;
		//unsigned int texture2;
		//unsigned int texture3;
		//unsigned int cubemapTexture;
		//std::shared_ptr<Shader> m_ShaderLight;
		//std::shared_ptr<Shader> m_ShaderTarget;
		//std::shared_ptr<Shader> m_ShaderModel;
		//std::shared_ptr<Shader> m_ShaderFramebuffer;
		//std::shared_ptr<Shader> m_ShaderSkybox;
		//std::shared_ptr<Shader> m_ShaderBase;
		// 
		//std::shared_ptr<VertexArray> m_VertexArray;
		//std::shared_ptr<VertexBuffer> m_VertexBuffer;
		//std::shared_ptr<IndexBuffer> m_IndexBuffer;
		//
		//std::shared_ptr<VertexArray> m_SkyboxVA;
		//std::shared_ptr<VertexBuffer> m_SkyboxVB;
		//std::shared_ptr<IndexBuffer> m_SkyboxIB;
		//
		//std::shared_ptr<VertexArray> m_BaseVA;
		//std::shared_ptr<VertexBuffer> m_BaseVB;
		//std::shared_ptr<IndexBuffer> m_BaseIB;
		//
		//std::shared_ptr<VertexArray> m_TargetVA;
		//
		//std::shared_ptr<VertexArray> quadVAO;
		//std::shared_ptr<VertexBuffer> quadVBO;
	};



}
