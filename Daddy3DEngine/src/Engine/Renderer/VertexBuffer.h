#pragma once
#include <glad/glad.h>

namespace DaddyEngine
{
	class VertexBuffer
	{
	public:
		VertexBuffer(const void* data, unsigned int size, GLenum usage = GL_STATIC_DRAW);
		~VertexBuffer();

		void Bind() const;
		void Unbind() const;

		void SetData(const void* data, unsigned int size, unsigned int offset);

	private:
		unsigned int m_RendererID;
	};
}