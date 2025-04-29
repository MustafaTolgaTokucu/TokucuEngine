#include "tkcpch.h"
#include "OpenGLContext.h"

#include "GLAD/glad.h"
#include "GLFW/glfw3.h"

//#include "Renderer/Texture.h"
//#include "Renderer/ShaderSrc.h"
//#define TINYOBJLOADER_IMPLEMENTATION
//#include "tiny_obj_loader.h" 


namespace Tokucu {

	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		TKC_CORE_ASSERT(windowHandle, "Window Handle Is Null!")
	}
	

	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		TKC_CORE_ASSERT(status, "Failed to initilalize GLAD!");
		TKC_CORE_INFO("Opengl renderer: {0}, {1} ", (const char*)glGetString(GL_VENDOR), (const char*)glGetString(GL_RENDERER));

		///////////////////////////////
		//////OpenGL Initializers//////////
		////////////////////////////////
		// 


		//TODO : Handle it while OPENGL Abstraction is done




		/*
		cubePositions = {
				glm::vec3(0.0f,  0.0f,  0.0f),
				glm::vec3(2.0f,  5.0f, -15.0f),
				glm::vec3(-1.5f, -2.2f, -2.5f),
				glm::vec3(-3.8f, -2.0f, -12.3f),
				glm::vec3(2.4f, -0.4f, -3.5f),
				glm::vec3(-1.7f,  3.0f, -7.5f),
				glm::vec3(1.3f, -2.0f, -2.5f),
				glm::vec3(1.5f,  2.0f, -2.5f),
				glm::vec3(1.5f,  0.2f, -1.5f),
				glm::vec3(-1.3f,  1.0f, -1.5f)
		};
		vertices = {
			- 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f ,      
			0.5f, 0.5f, 0.0f,0.0f, 0.0f, 1.0f, 1.0f, 1.0f  ,      
			-0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f  ,     
		
			-0.5f, -0.5f, -0.5f , 1.0f, 0.0f, 0.0f, 0.0f, 0.0f  ,
			0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f  ,    
			0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f   ,    
			-0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f  ,    
		};
		
		indices = {
			0, 1, 2, 2, 3, 0,
			4, 5, 6, 6, 7, 4
		};
		 float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
			// positions   // texCoords
			-1.0f,  1.0f,  0.0f, 1.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,
			 1.0f,  1.0f,  1.0f, 1.0f
		};
		unsigned int quadIndices[] = {0,2};
		
		
		target = { cubeVertices, vertexBuffer, vertexBufferMemory, cubeIndices, indexBuffer, indexBufferMemory, bindingDescriptions, attributeDescriptions, pipeline, pipelineLayout };
		std::vector<std::string> faces{
				"C:/Users/tokuc/source/repos/Tokucu Engine/Tokucu Engine/vendor/images/skybox/right.jpg",
				"C:/Users/tokuc/source/repos/Tokucu Engine/Tokucu Engine/vendor/images/skybox/left.jpg",
				"C:/Users/tokuc/source/repos/Tokucu Engine/Tokucu Engine/vendor/images/skybox/top.jpg",
				"C:/Users/tokuc/source/repos/Tokucu Engine/Tokucu Engine/vendor/images/skybox/bottom.jpg",
				"C:/Users/tokuc/source/repos/Tokucu Engine/Tokucu Engine/vendor/images/skybox/front.jpg",
				"C:/Users/tokuc/source/repos/Tokucu Engine/Tokucu Engine/vendor/images/skybox/back.jpg"
		};
		float skyboxVertices[] = {
			// Positions
			// Front face
			-1.0f, -1.0f,  1.0f,  // Bottom-left
			 1.0f, -1.0f,  1.0f,  // Bottom-right
			 1.0f,  1.0f,  1.0f,  // Top-right
			-1.0f,  1.0f,  1.0f,  // Top-left
		
			// Back face
			-1.0f, -1.0f, -1.0f,  // Bottom-left
			 1.0f, -1.0f, -1.0f,  // Bottom-right
			 1.0f,  1.0f, -1.0f,  // Top-right
			-1.0f,  1.0f, -1.0f,  // Top-left
		
			// Left face
			-1.0f, -1.0f, -1.0f,  // Bottom-left
			-1.0f,  1.0f, -1.0f,  // Top-left
			-1.0f,  1.0f,  1.0f,  // Top-right
			-1.0f, -1.0f,  1.0f,  // Bottom-right
		
			// Right face
			 1.0f, -1.0f, -1.0f,  // Bottom-left
			 1.0f,  1.0f, -1.0f,  // Top-left
			 1.0f,  1.0f,  1.0f,  // Top-right
			 1.0f, -1.0f,  1.0f,  // Bottom-right
		
			 // Top face
			 -1.0f,  1.0f, -1.0f,  // Bottom-left
			  1.0f,  1.0f, -1.0f,  // Bottom-right
			  1.0f,  1.0f,  1.0f,  // Top-right
			 -1.0f,  1.0f,  1.0f,  // Top-left
		
			 // Bottom face
			 -1.0f, -1.0f, -1.0f,  // Bottom-left
			  1.0f, -1.0f, -1.0f,  // Bottom-right
			  1.0f, -1.0f,  1.0f,  // Top-right
			 -1.0f, -1.0f,  1.0f   // Top-left
		};
		unsigned int skyboxIndices[] = {
			// Front face
			0, 1, 2, 0, 2, 3,
			// Back face
			4, 5, 6, 4, 6, 7,
			// Left face
			8, 9, 10, 8, 10, 11,
			// Right face
			12, 13, 14, 12, 14, 15,
			// Top face
			16, 17, 18, 16, 18, 19,
			// Bottom face
			20, 21, 22, 20, 22, 23
		};
		glEnable(GL_MULTISAMPLE);
		//Light object
		m_VertexArray.reset(VertexArray::Create());
		m_VertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));
		m_IndexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices)));
		BufferLayout layout = { {ShaderDataType::Float3, "a_Position"} ,{ShaderDataType::Float3, "a_Normal"},{ShaderDataType::Float2,"a_Texture"} };
		m_VertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);
		//For target object get affected by light
		m_TargetVA.reset(VertexArray::Create());
		std::shared_ptr<VertexBuffer> TargetVB;
		std::shared_ptr<IndexBuffer> TargetIB;
		TargetVB.reset(VertexBuffer::Create(vertices, sizeof(vertices)));
		TargetIB.reset(IndexBuffer::Create(indices, sizeof(indices)));
		TargetVB->SetLayout({ {ShaderDataType::Float3, "a_Position"} ,{ ShaderDataType::Float3, "a_Normal" },{ShaderDataType::Float2,"a_Texture"}});
		m_TargetVA-> AddVertexBuffer(TargetVB);
		m_TargetVA->SetIndexBuffer(TargetIB);
		//for base object
		m_BaseVA.reset(VertexArray::Create());
		m_BaseVB.reset(VertexBuffer::Create(vertices, sizeof(vertices)));
		m_BaseIB.reset(IndexBuffer::Create(indices, sizeof(indices)));
		m_BaseVB->SetLayout({ {ShaderDataType::Float3, "a_Position"} ,{ ShaderDataType::Float3, "a_Normal" },{ShaderDataType::Float2,"a_Texture"} });
		m_BaseVA->AddVertexBuffer(m_BaseVB);
		m_BaseVA->SetIndexBuffer(m_BaseIB);
		//For framebuffer
		quadVAO.reset(VertexArray::Create());
		quadVBO.reset(VertexBuffer::Create(quadVertices, sizeof(quadVertices)));
		quadVBO->SetLayout({ {ShaderDataType::Float2, "a_Position"}, {ShaderDataType::Float2,"a_Texture"} });
		quadVAO->AddVertexBuffer(quadVBO);
		std::shared_ptr<IndexBuffer> quadIB;
		quadIB.reset(IndexBuffer::Create(quadIndices, sizeof(quadIndices)));
		quadVAO->SetIndexBuffer(quadIB);
		//For skybox
		m_SkyboxVA.reset(VertexArray::Create());
		m_SkyboxVB.reset(VertexBuffer::Create(skyboxVertices, sizeof(skyboxVertices)));
		m_SkyboxIB.reset(IndexBuffer::Create(skyboxIndices, sizeof(skyboxIndices)));
		m_SkyboxVB->SetLayout({ {ShaderDataType::Float3, "a_Position"} });
		m_SkyboxVA->AddVertexBuffer(m_SkyboxVB);
		m_SkyboxVA->SetIndexBuffer(m_SkyboxIB);
		
		////////////////////////////////////////////////////////////////////////////	
		///TEXTURE LOADING
		////////////////////////////////////////////////////////////////////////////
		TextureLoader texLoader;
		texture = texLoader.LoadTexture("C:/Users/tokuc/source/repos/Tokucu Engine/Tokucu Engine/vendor/images/Lambda.jpg");
		texture2 = texLoader.LoadTexture("C:/Users/tokuc/source/repos/Tokucu Engine/Tokucu Engine/vendor/images/Lambda.jpg");
		texture3 = texLoader.LoadTexture("C:/Users/tokuc/source/repos/Tokucu Engine/Tokucu Engine/vendor/images/Matrix.jpg");
		cubemapTexture = texLoader.loadCubemap(faces);
		frameBufferTexture = texLoader.LoadFrameTexture();
		////////////////////////////////////////////////////////////////////////////	
		///Shader LOADING
		////////////////////////////////////////////////////////////////////////////
		ShaderSrc ShaderSrcLoader;
		m_ShaderFramebuffer.reset(new Shader(ShaderSrcLoader.FramebufferVertex, ShaderSrcLoader.FramebufferFragment));
		m_ShaderLight.reset(new Shader(ShaderSrcLoader.vertexSrc, ShaderSrcLoader.fragmentSrc));
		m_ShaderTarget.reset(new Shader(ShaderSrcLoader.vertexSrc, ShaderSrcLoader.fragmentSrcTarget));
		m_ShaderModel.reset(new Shader(ShaderSrcLoader.vertexSrc, ShaderSrcLoader.fragmentSrcTarget));
		m_ShaderSkybox.reset(new Shader(ShaderSrcLoader.skyboxVertexShader, ShaderSrcLoader.skyboxFragmentShader));
		m_ShaderBase.reset(new Shader(ShaderSrcLoader.vertexSrc, ShaderSrcLoader.fragmentSrcTarget));
		////////////////////////////////////////////////////////////////////////////	
		///FRAMEBUFFER ADJUSTMENTS
		////////////////////////////////////////////////////////////////////////////
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBufferTexture, 0);
		
		unsigned int rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Window->GetHeight(),m_Window->GetWidth());
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1920, 1080);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		unsigned int depthMapFBO;
		glGenFramebuffers(1, &depthMapFBO);
		const unsigned int SHADOW_WIDTH = 1920, SHADOW_HEIGHT = 1080;
		
		unsigned int depthMap;
		glGenTextures(1, &depthMap);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
			SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		//////////////////////////////////////////////////
		///UNIFORM BUFFER OBJECT
		//////////////////////////////////////////////////
		unsigned int uniformBlockIndexGreen = glGetUniformBlockIndex(m_ShaderLight->GetShaderID(), "Matrices");
		unsigned int uniformBlockIndexBlue = glGetUniformBlockIndex(m_ShaderTarget->GetShaderID(), "Matrices");
		unsigned int uniformBlockIndexYellow = glGetUniformBlockIndex(m_ShaderModel->GetShaderID(), "Matrices");
		unsigned int uniformBlockIndexDark = glGetUniformBlockIndex(m_ShaderBase->GetShaderID(), "Matrices");
		glUniformBlockBinding(m_ShaderLight->GetShaderID(), uniformBlockIndexGreen, 0);
		glUniformBlockBinding(m_ShaderTarget->GetShaderID(), uniformBlockIndexBlue, 0);
		glUniformBlockBinding(m_ShaderModel->GetShaderID(), uniformBlockIndexYellow, 0);
		glUniformBlockBinding(m_ShaderBase->GetShaderID(), uniformBlockIndexDark, 0);
		
		glGenBuffers(1, &uboMatrices);
		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
		glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4)+sizeof(glm::uint), NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4) + sizeof(glm::uint));
		
		glGenBuffers(1, &ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
		//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo); // Binding index 0 matches GLSL
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo);
		
		m_ShaderTarget->Bind();
		m_ShaderTarget->SetInt("material.diffuse",0);
		m_ShaderTarget->SetInt("material.specular",1);
		m_ShaderTarget->SetInt("material.emission",2);
		m_ShaderTarget->SetInt("skybox", 3);
		m_ShaderSkybox->Bind();
		m_ShaderSkybox->SetInt("skybox", 3);
		m_ShaderFramebuffer->Bind();
		m_ShaderFramebuffer->SetInt("screenTexture", 4);
		ourModel.reset(new Model("C:/Users/tokuc/source/repos/Tokucu Engine/Tokucu Engine/vendor/images/backpack/backpack.obj"));
		
		m_ShaderBase->Bind();
		m_ShaderBase->SetInt("material.diffuse", 0);
		m_ShaderBase->SetInt("material.specular", 1);
		m_ShaderBase->SetInt("material.emission", 2);*/
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}

}