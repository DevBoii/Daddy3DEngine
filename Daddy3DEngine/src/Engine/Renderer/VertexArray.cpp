#include "VertexArray.h"
#include "VertexBuffer.h"


namespace DaddyEngine
{
	VertexArray::VertexArray()
	{
		glGenVertexArrays(1, &m_RendererID);
	}

	VertexArray::~VertexArray()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void VertexArray::Bind() const
	{
		glBindVertexArray(m_RendererID);
	}

	void VertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void VertexArray::AddBuffer(const VertexBuffer& vbo, const std::vector<BufferElement>& elements)
	{
		Bind();	// First Bind VAO
		vbo.Bind();		// Then VBO

		unsigned int m_stride = 0;
		for (auto& attribute : elements)
		{
			m_stride += attribute.count * attribute.GetSizeofType(attribute.type);
		}

		size_t offset = 0;
		for (int i = 0; i < elements.size(); i++)
		{
			glEnableVertexAttribArray(i);
			glVertexAttribPointer(
				i,														// index
				elements[i].count,										// Size = Component Count (e.g., 3 for XYZ)
				elements[i].type,										// type
				elements[i].normalized ? GL_TRUE : GL_FALSE,									// normalized
				m_stride,												// stride
				(const void*)offset);									// pointer

			offset += elements[i].count * elements[i].GetSizeofType(elements[i].type);
		}
	}
}