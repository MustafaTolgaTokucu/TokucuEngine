#pragma once


#include <glm/glm.hpp>
namespace Tokucu {

	class Shader
	{
	public:
		Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
		~Shader();

		void Bind();
		void Unbind();
	/// <summary>
	/// MTT variables
	/// </summary>
		int GetUniformLocation(const char* uniformVar);
		void ColorUpdate(int VertexColorLocation, float RedValue, float GreenValue, float BlueValue);
		void SetUniform(int VertexColorLocation);
		void SetMat4(const char* uniformVar, const glm::mat4& mat);
		void SetVec3(const char* uniformVar, const glm::vec3& value);
		void SetVec3(const char* uniformVar, float x, float y, float z);
		void SetFloat(const char* uniformVar, float value);
		void SetInt(const char* uniformVar, int value);
		
		uint32_t GetShaderID() { return m_RendererID; }

	private:
		uint32_t m_RendererID;


	};


}


