#include "IndexBuffer.h"

#include <glad/glad.h>

namespace DaddyEngine
{
	IndexBuffer::IndexBuffer(const unsigned int* indices, unsigned int count)
	{
		m_count = count;
		glGenBuffers(1, &m_RendererID);
		Bind();
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), indices, GL_STATIC_DRAW);
	}

	IndexBuffer::~IndexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void IndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	}

	void IndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	unsigned int IndexBuffer::GetCount() const
	{
		return m_count;
	}

}