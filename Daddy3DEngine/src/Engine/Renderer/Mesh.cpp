#include "Mesh.h"
#include "Engine/Renderer/Material.h"

namespace DaddyEngine
{
	Mesh::Mesh(const MeshData& data, bool isStaticMesh)
		: m_IsStatic(isStaticMesh), m_CPUVertices(data.Vertices)
	{
		m_IndexCount = (unsigned int)data.Indices.size();

		m_VAO = std::make_shared<VertexArray>();
		m_VAO->Bind();

		//	Select buffer usage hint based on boolean flag
		GLenum usage = m_IsStatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW;

		m_VBO = std::make_shared<VertexBuffer>(&data.Vertices[0], data.Vertices.size() * sizeof(Vertex), usage);

		std::vector<BufferElement> layout = {
			{ GL_FLOAT, 3, false }, // Layout 0: Position (3 floats - XYZ)
			{ GL_FLOAT, 3, false }, // Layout 1: Normal (3 floats - XYZ)  
			{ GL_FLOAT, 2, false },  // Layout 2: TexCoords (2 floats - UV) 
			{ GL_FLOAT, 3, false } // Layout 3: Color (3 floats)
		};

		m_VAO->AddBuffer(*m_VBO, layout);
		m_IBO = std::make_shared<IndexBuffer>(&data.Indices[0], m_IndexCount);
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

	void Mesh::UpdateVertices(const std::vector<Vertex>& vertices)
	{
		if (m_IsStatic) return;

		m_CPUVertices = vertices;

		if (m_VBO)
			m_VBO->SetData(m_CPUVertices.data(), m_CPUVertices.size() * sizeof(Vertex), 0);	// Re-uploads the modified RAM array to VRAM in a single contiguous GPU write
	}
}