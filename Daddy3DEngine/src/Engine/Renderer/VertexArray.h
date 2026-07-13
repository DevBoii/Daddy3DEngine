#pragma once
#include "VertexBuffer.h"

#include <glad/glad.h>
#include <vector>
#include <cassert>

namespace DaddyEngine
{
	// more efficient by removing padding
	// total 12 byte
	struct BufferElement 
	{
		unsigned int type;	// 4 byte
		unsigned int count;	// 4  byte
		bool normalized;	// 1 byte
		// 9 byte and 3 byte enmy space

		static unsigned int GetSizeofType(unsigned int type)
		{
			switch (type)
			{
			case GL_FLOAT: return 4;
			case GL_UNSIGNED_INT: return 4;
			case GL_UNSIGNED_BYTE: return 1;
			}
			assert(false);
			return 0;
		}
	};

	class VertexArray
	{
	private:
		unsigned int m_RendererID;

	public:
		VertexArray();
		~VertexArray();

		void Bind() const;
		void Unbind() const;

		void AddBuffer(const VertexBuffer& vbo, const std::vector<BufferElement>& elements);
	};
}