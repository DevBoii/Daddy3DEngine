#include "Mesh.h"

namespace DaddyEngine
{
	Mesh::Mesh(const MeshData& data)
	{
		m_IndexCount = (unsigned int)data.Indices.size();

		m_VAO = std::make_unique<VertexArray>();
		m_VAO->Bind();

		m_VBO = std::make_unique<VertexBuffer>(&data.Vertices[0], data.Vertices.size() * sizeof(Vertex));

		std::vector<BufferElement> layout = {
			{ GL_FLOAT, 3, false }, // Layout 0: Position (3 floats - XYZ)
			{ GL_FLOAT, 3, false }, // Layout 1: Normal (3 floats - XYZ)  
			{ GL_FLOAT, 2, false },  // Layout 2: TexCoords (2 floats - UV) 
			{ GL_FLOAT, 3, false } // Layout 3: Color (3 floats)
		};

		m_VAO->AddBuffer(*m_VBO, layout);
		m_IBO = std::make_unique<IndexBuffer>(&data.Indices[0], m_IndexCount);
		m_VAO->Unbind();
	}

	void Mesh::Bind()
	{
		if(m_VAO)
			m_VAO->Bind();
		if(m_VBO)
			m_VBO->Bind();
		if(m_IBO)
			m_IBO->Bind();
	}
}