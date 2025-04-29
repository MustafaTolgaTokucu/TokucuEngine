#include "tkcpch.h"
#include "Shader.h"

#include "GLAD/glad.h"

namespace Tokucu {



	Shader::Shader(const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		

		// Create an empty vertex shader handle
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		// Send the vertex shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		const GLchar* source = vertexSrc.c_str();
		glShaderSource(vertexShader, 1, &source, 0);

		// Compile the vertex shader
		glCompileShader(vertexShader);

		GLint isCompiled = 0;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(vertexShader);

			// Use the infoLog as you see fit.

			TKC_CORE_ERROR("{0}", infoLog.data());
			TKC_CORE_ASSERT(false, "Vertex shader compilation failure!")


			// In this simple program, we'll just leave
			return;
		}

		// Create an empty fragment shader handle
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		// Send the fragment shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		source = fragmentSrc.c_str();
		glShaderSource(fragmentShader, 1, &source, 0);

		// Compile the fragment shader
		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(fragmentShader);
			// Either of them. Don't leak shaders.
			glDeleteShader(vertexShader);

			// Use the infoLog as you see fit.
			TKC_CORE_ERROR("{0}", infoLog.data());
			TKC_CORE_ASSERT(false, "Fragment shader compilation failure!")
			// In this simple program, we'll just leave
			return;
		}

		// Vertex and fragment shaders are successfully compiled.
		// Now time to link them together into a program.
		// Get a program object.

		m_RendererID = glCreateProgram();
		GLuint program = m_RendererID;

		// Attach our shaders to our program
		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);

		// Link our program
		glLinkProgram(program);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(program);
			// Don't leak shaders either.
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			// Use the infoLog as you see fit.
			TKC_CORE_ERROR("{0}", infoLog.data());
			TKC_CORE_ASSERT(false, "Shader linking failure!")
			// In this simple program, we'll just leave
			return;
		}
		
		// Always detach shaders after a successful link.
		glDetachShader(program, vertexShader);
		glDetachShader(program, fragmentShader);
	}

	Shader::~Shader()
	{
		glDeleteProgram(m_RendererID);
	}

	void Shader::Bind()
	{
		glUseProgram(m_RendererID);
	}

	void Shader::Unbind()
	{
		glUseProgram(0);

	}

	int Shader::GetUniformLocation(const char* uniformVar)
	{
		if (m_RendererID==0)
		{
		
			return 0;
		}
		
		return glGetUniformLocation(m_RendererID, uniformVar);
	}

	void Shader::ColorUpdate(int VertexColorLocation,float RedValue, float GreenValue, float BlueValue)
	{
		glUniform4f(VertexColorLocation, RedValue, GreenValue, BlueValue,1);
	}

	void Shader::SetUniform(int VertexColorLocation)
	{
		glUniform1i(VertexColorLocation, 0);
	}

	void Shader::SetMat4(const char* uniformVar, const glm::mat4& mat)
	{
		glUniformMatrix4fv(GetUniformLocation(uniformVar),1, GL_FALSE, &mat[0][0]);
	}

	void Shader::SetVec3(const char* uniformVar, const glm::vec3& value)
	{
		glUniform3fv(GetUniformLocation(uniformVar), 1, &value[0]);
	}

	void Shader::SetVec3(const char* uniformVar, float x, float y, float z)
	{
		glUniform3f(GetUniformLocation(uniformVar), x, y, z);
	}

	void Shader::SetFloat(const char* uniformVar, float value)
	{
		glUniform1f(GetUniformLocation(uniformVar), value);
	}

	void Shader::SetInt(const char* uniformVar, int value)
	{
		glUniform1i(GetUniformLocation(uniformVar), value);
	}

	


}