#include "tkcpch.h"
#include "OpenGLRendererAPI.h"

#include "GLAD/glad.h"

namespace Tokucu {

	//Main Renderer for OPENGL 
	//TODO: Correct abstraction


	void OpenGLRendererAPI::Init(const std::shared_ptr<Window>& window)
	{
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::Clear()
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_STENCIL_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		
	}

	void OpenGLRendererAPI::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray)
	{
		glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererAPI::Render()
	{
		//float timeValue = glfwGetTime();
		//float redvalue = 10 * sin(glm::radians(timeValue)) * cos(glm::radians(timeValue));
		// ////////////////////////////
			///DRAWING 10 TEXTURED CUBES
			////////////////////////////

		//glm::vec3 pointLightPositions[] = {
		//	glm::vec3(0.7f,  0.2f,  2.0f),
		//	glm::vec3(2.3f, -3.3f, -4.0f),
		//	glm::vec3(-4.0f,  2.0f, -12.0f),
		//	glm::vec3(0.0f,  0.0f, 2.0f)
		//};
		//glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			//RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.5f, 1 });
			//RenderCommand::Clear();
			//Renderer::BeginScene();
			//glActiveTexture(GL_TEXTURE0);
			//glBindTexture(GL_TEXTURE_2D, texture);
			//glActiveTexture(GL_TEXTURE1);
			//glBindTexture(GL_TEXTURE_2D, texture2);
			//glActiveTexture(GL_TEXTURE2);
			//glBindTexture(GL_TEXTURE_2D, texture3);
			//glActiveTexture(GL_TEXTURE3);
			//glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
			//glActiveTexture(GL_TEXTURE4);
			//glBindTexture(GL_TEXTURE_2D, frameBufferTexture);
			//
			//glm::mat4 view = camera->GetView();
			//
			//glm::mat4 projection = glm::perspective(glm::radians(75.0f), 1920 / 1080.f, 0.1f, 100.0f);
			//
			//glm::vec3 LightSrcColor = glm::vec3(0.5f,0.5f,0.0f);
			//float LightIntensity = 1.f;
			//glm::vec3 ambientColor = glm::vec3(0.05f);
			//glm::vec3 diffuseColor = glm::vec3(0.8f);
			//glm::vec3 LightSourcePosition = glm::vec3(0.0f, 0.0f, 2.0f);
			//glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f);
			//
			////color, position, ambient, diffuse, specular, constant, linear, quadratic
			//std::vector<LightAttributes> pointLights = {
			//	{LightSrcColor,  LightSourcePosition, ambientColor,diffuseColor,specular,1.f,0.09f,0.032f}
			//};
			//
			//// Update numLights uniform
			//size_t pointLightCount = pointLights.size();
			////glUniform1i(glGetUniformLocation(shaderProgram, "numLights"), numLights);
			//
			//glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
			//glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
			//glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
			//glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::uint), sizeof(glm::uint), &pointLightCount);
			//glBindBuffer(GL_UNIFORM_BUFFER, 0);
			//
			//glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
			//glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(LightAttributes) * pointLights.size(), pointLights.data(), GL_DYNAMIC_DRAW);
			//
			//
			//m_ShaderLight->Bind();
			//unsigned int transformlocation = m_ShaderLight->GetUniformLocation("transform");
			//m_ShaderLight->SetVec3("LightSourceColor", LightSrcColor);
			//for (size_t i = 0; i < 4; i++)
			//{
			//	glm::mat4 model = glm::rotate(model, timeValue * glm::radians(45.0f), glm::vec3(0.5f, 1.0f, 0.0f));
			//	glm::mat4 model2 = glm::mat4(1.0f);
			//	model2 = glm::translate(model2, LightSourcePosition);
			//	model2 = glm::scale(model2, glm::vec3(0.2f)); // Make it a smaller cube
			//	m_ShaderLight->SetMat4("transform", model2);
			//}
			//Renderer::Submit(m_VertexArray);
			//
			//m_ShaderTarget->Bind();
			//m_ShaderTarget->SetVec3("viewPos", camera->GetPosition());
			//m_ShaderTarget->SetVec3("material.ambient", 1,1,1);
			//m_ShaderTarget->SetFloat("material.shininess", 128);
			////Directional Light additional attributes
			////m_ShaderTarget->SetVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
			//// spotLight additional attributes
			////m_ShaderTarget->SetVec3("spotlight.position", camera->GetPosition()); // for flashlight
			////m_ShaderTarget->SetVec3("spotlight.direction", camera->Front);
			////m_ShaderTarget->SetFloat("spotlight.cutOff", glm::cos(glm::radians(12.5f)));
			////m_ShaderTarget->SetFloat("spotlight.outerCutOff", glm::cos(glm::radians(15.0f)));
			//for (unsigned int i = 0; i < 10; i++)
			//{
			//	glm::mat4 model3 = glm::mat4(1.0f);
			//	model3 = glm::translate(model3, cubePositions[i]);
			//	float angle = 20.0f * i + 5;
			//	if (i % 3 == 0)  // every 3rd iteration (including the first) we set the angle using GLFW's time function.
			//		angle = glfwGetTime() * 25.0f;
			//	model3 = glm::rotate(model3, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			//	m_ShaderTarget->SetMat4("transform", model3);
			//	Renderer::Submit(m_TargetVA);
			//}
			//
			//m_ShaderBase->Bind();
			//
			//glm::mat4 model4 = glm::mat4(1.0f);
			//model4 = glm::translate(model4, glm::vec3(0, -7,0));
			//model4 = glm::scale(model4, glm::vec3(20,0.5,20));
			//m_ShaderBase->SetMat4("transform", model4);
			//m_ShaderBase->SetVec3("viewPos", camera->GetPosition());
			//m_ShaderBase->SetVec3("material.ambient", 1, 1, 1);
			//m_ShaderBase->SetFloat("material.shininess", 128);
			//Renderer::Submit(m_BaseVA);
			//
			//m_ShaderModel->Bind();
			//m_ShaderModel->SetVec3("viewPos", camera->GetPosition());
			//m_ShaderModel->SetVec3("material.ambient", 1, 1, 1);
			//m_ShaderModel->SetFloat("material.shininess", 128);
			//// render the loaded model
			//glm::mat4 model31 = glm::mat4(1.0f);
			//model31 = glm::translate(model31, glm::vec3(0.0f, 0.0f, 13.0f)); // translate it down so it's at the center of the scene
			//model31 = glm::scale(model31, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
			//m_ShaderModel->SetMat4("transform", model31);
			//ourModel->Draw(*m_ShaderModel);
			//
			//glDepthFunc(GL_LEQUAL);
			//m_ShaderSkybox->Bind();
			//glm::mat4 skyboxView = glm::mat4(glm::mat3(camera->GetView()));
			//m_ShaderSkybox->SetMat4("view2", skyboxView);
			//Renderer::Submit(m_SkyboxVA);
			//glDepthFunc(GL_LESS);
			//
			////now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
			//glBindFramebuffer(GL_FRAMEBUFFER, 0);
			//glDisable(GL_DEPTH_TEST);
			//glDisable(GL_STENCIL_TEST);
			//// disable depth test so screen-space quad isn't discarded due to depth test.
			//// clear all relevant buffers
			//glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
			//glClear(GL_COLOR_BUFFER_BIT);
			//
			//m_ShaderFramebuffer->Bind();
			//Renderer::Submit(quadVAO);
	}

}