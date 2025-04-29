#include "tkcpch.h"
#include "OpenGLVertexArray.h"

#include "GLAD/glad.h"
namespace Tokucu {

	static GLenum ShaderDataTypetoOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
		case Tokucu::ShaderDataType::None:    return GL_FLOAT;
		case Tokucu::ShaderDataType::Float:	  return GL_FLOAT;
		case Tokucu::ShaderDataType::Float2:  return GL_FLOAT;
		case Tokucu::ShaderDataType::Float3:  return GL_FLOAT;
		case Tokucu::ShaderDataType::Float4:  return GL_FLOAT;
		case Tokucu::ShaderDataType::Mat3:	  return GL_FLOAT;
		case Tokucu::ShaderDataType::Mat4:	  return GL_FLOAT;
		case Tokucu::ShaderDataType::Int:	  return GL_INT;
		case Tokucu::ShaderDataType::Int2:	  return GL_INT;
		case Tokucu::ShaderDataType::Int3:	  return GL_INT;
		case Tokucu::ShaderDataType::Int4:	  return GL_INT;
		case Tokucu::ShaderDataType::Bool:    return GL_BOOL;
		}
		TKC_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}
	Tokucu::OpenGLVertexArray::OpenGLVertexArray()
	{
		glCreateVertexArrays(1, &m_RendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void Tokucu::OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(m_RendererID);
	}

	void Tokucu::OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void Tokucu::OpenGLVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
	{

		TKC_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");
		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();


		//This function is automated 
		//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		
		const auto& layout = vertexBuffer->GetLayout();
		for (const auto& element : layout)
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index,
				element.GetComponentCount(),
				ShaderDataTypetoOpenGLBaseType(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(const void*)element.Offset);
			index++;
		}

		m_VertexBuffers.push_back(vertexBuffer);
	}
	void Tokucu::OpenGLVertexArray::AddInstancedVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
	{

		TKC_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");
		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();

		//This function is automated 
		//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		uint32_t index = 0;
		const auto& layout = vertexBuffer->GetLayout();
		for (const auto& element : layout)
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index,
				element.GetComponentCount(),
				ShaderDataTypetoOpenGLBaseType(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(const void*)element.Offset);
			index++;
		}

		m_VertexBuffers.push_back(vertexBuffer);
	}
	void Tokucu::OpenGLVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
	{
		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;
	}

}