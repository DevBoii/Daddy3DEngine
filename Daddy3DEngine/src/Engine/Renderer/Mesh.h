#pragma once

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

#include <memory>
#include <glm/glm.hpp>

namespace DaddyEngine
{
    // Forward declaration to prevent circular dependency
    class Material;

	class Shape
	{
		bool hasShape = true;
	public:
		virtual const float* getVertices() const = 0;
		virtual  const unsigned int getVertexSize() const = 0;

		virtual  const unsigned int* getIndices() const = 0;
		virtual const unsigned int getIndexCount() const = 0;
	};
    class CubeShape : public Shape
    {
    public:
        // 24 vertices * 9 floats (3 Position + 3 Color + 3 Normal)
        float vertices[216] = {
            // ---- Positions ----         ---- Colors ----           ---- Normals ----

            // Front Face
            -0.5f, -0.5f,  0.5f,          0.8f, 0.8f, 0.8f,          0.0f,  0.0f,  1.0f,
             0.5f, -0.5f,  0.5f,          0.8f, 0.8f, 0.8f,          0.0f,  0.0f,  1.0f,
             0.5f,  0.5f,  0.5f,          0.8f, 0.8f, 0.8f,          0.0f,  0.0f,  1.0f,
            -0.5f,  0.5f,  0.5f,          0.8f, 0.8f, 0.8f,          0.0f,  0.0f,  1.0f,

            // Back Face
            -0.5f, -0.5f, -0.5f,          0.8f, 0.8f, 0.8f,          0.0f,  0.0f, -1.0f,
             0.5f, -0.5f, -0.5f,          0.8f, 0.8f, 0.8f,          0.0f,  0.0f, -1.0f,
             0.5f,  0.5f, -0.5f,          0.8f, 0.8f, 0.8f,          0.0f,  0.0f, -1.0f,
            -0.5f,  0.5f, -0.5f,          0.8f, 0.8f, 0.8f,          0.0f,  0.0f, -1.0f,

            // Top Face
            -0.5f,  0.5f,  0.5f,          0.8f, 0.8f, 0.8f,          0.0f,  1.0f,  0.0f,
             0.5f,  0.5f,  0.5f,          0.8f, 0.8f, 0.8f,          0.0f,  1.0f,  0.0f,
             0.5f,  0.5f, -0.5f,          0.8f, 0.8f, 0.8f,          0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,          0.8f, 0.8f, 0.8f,          0.0f,  1.0f,  0.0f,

            // Bottom Face
            -0.5f, -0.5f, -0.5f,          0.8f, 0.8f, 0.8f,          0.0f, -1.0f,  0.0f,
             0.5f, -0.5f, -0.5f,          0.8f, 0.8f, 0.8f,          0.0f, -1.0f,  0.0f,
             0.5f, -0.5f,  0.5f,          0.8f, 0.8f, 0.8f,          0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,          0.8f, 0.8f, 0.8f,          0.0f, -1.0f,  0.0f,

            // Right Face
             0.5f, -0.5f,  0.5f,          0.8f, 0.8f, 0.8f,          1.0f,  0.0f,  0.0f,
             0.5f, -0.5f, -0.5f,          0.8f, 0.8f, 0.8f,          1.0f,  0.0f,  0.0f,
             0.5f,  0.5f, -0.5f,          0.8f, 0.8f, 0.8f,          1.0f,  0.0f,  0.0f,
             0.5f,  0.5f,  0.5f,          0.8f, 0.8f, 0.8f,          1.0f,  0.0f,  0.0f,

             // Left Face
             -0.5f, -0.5f, -0.5f,          0.8f, 0.8f, 0.8f,         -1.0f,  0.0f,  0.0f,
             -0.5f, -0.5f,  0.5f,          0.8f, 0.8f, 0.8f,         -1.0f,  0.0f,  0.0f,
             -0.5f,  0.5f,  0.5f,          0.8f, 0.8f, 0.8f,         -1.0f,  0.0f,  0.0f,
             -0.5f,  0.5f, -0.5f,          0.8f, 0.8f, 0.8f,         -1.0f,  0.0f,  0.0f
        };

        unsigned int indices[36] = {
             0,  1,  2,  2,  3,  0,
             4,  5,  6,  6,  7,  4,
             8,  9, 10, 10, 11,  8,
            12, 13, 14, 14, 15, 12,
            16, 17, 18, 18, 19, 16,
            20, 21, 22, 22, 23, 20
        };

        const float* getVertices() const override { return vertices; }
        const unsigned int getVertexSize() const override { return sizeof(vertices); }
        const unsigned int* getIndices() const override { return indices; }
        const unsigned int getIndexCount() const override { return sizeof(indices) / sizeof(unsigned int); }
    };
    class PyramidShape : public Shape
    {
    public:
        // 16 vertices * 9 floats (3 Pos + 3 Color + 3 Normal) = 144 floats
        float vertices[144] = {
            // ---- Positions ----          ---- Colors ----          ---- Normals ----

            // Front Face
             0.0f,  0.5f,  0.0f,         0.8f, 0.8f, 0.8f,         0.0f,  0.447f,  0.894f,
            -0.5f, -0.5f,  0.5f,         0.8f, 0.8f, 0.8f,         0.0f,  0.447f,  0.894f,
             0.5f, -0.5f,  0.5f,         0.8f, 0.8f, 0.8f,         0.0f,  0.447f,  0.894f,

             // Right Face
              0.0f,  0.5f,  0.0f,         0.8f, 0.8f, 0.8f,         0.894f, 0.447f,  0.0f,
              0.5f, -0.5f,  0.5f,         0.8f, 0.8f, 0.8f,         0.894f, 0.447f,  0.0f,
              0.5f, -0.5f, -0.5f,         0.8f, 0.8f, 0.8f,         0.894f, 0.447f,  0.0f,

              // Back Face
               0.0f,  0.5f,  0.0f,         0.8f, 0.8f, 0.8f,         0.0f,  0.447f, -0.894f,
               0.5f, -0.5f, -0.5f,         0.8f, 0.8f, 0.8f,         0.0f,  0.447f, -0.894f,
              -0.5f, -0.5f, -0.5f,         0.8f, 0.8f, 0.8f,         0.0f,  0.447f, -0.894f,

              // Left Face
               0.0f,  0.5f,  0.0f,         0.8f, 0.8f, 0.8f,        -0.894f, 0.447f,  0.0f,
              -0.5f, -0.5f, -0.5f,         0.8f, 0.8f, 0.8f,        -0.894f, 0.447f,  0.0f,
              -0.5f, -0.5f,  0.5f,         0.8f, 0.8f, 0.8f,        -0.894f, 0.447f,  0.0f,

              // Base Bottom Face
              -0.5f, -0.5f,  0.5f,         0.8f, 0.8f, 0.8f,         0.0f, -1.0f,  0.0f,
               0.5f, -0.5f,  0.5f,         0.8f, 0.8f, 0.8f,         0.0f, -1.0f,  0.0f,
               0.5f, -0.5f, -0.5f,         0.8f, 0.8f, 0.8f,         0.0f, -1.0f,  0.0f,
              -0.5f, -0.5f, -0.5f,         0.8f, 0.8f, 0.8f,         0.0f, -1.0f,  0.0f
        };

        unsigned int indices[18] = {
            0, 1, 2,      // Front
            3, 4, 5,      // Right
            6, 7, 8,      // Back
            9, 10, 11,    // Left
            12, 14, 13,   // Base Triangle 1
            12, 15, 14    // Base Triangle 2
        };

        const float* getVertices() const override { return vertices; }
        const unsigned int getVertexSize() const override { return sizeof(vertices); }
        const unsigned int* getIndices() const override { return indices; }
        const unsigned int getIndexCount() const override { return sizeof(indices) / sizeof(unsigned int); }
    };
    class PlaneShape : public Shape
    {
    public:
        // 4 vertices * 9 floats = 36 floats total
        float vertices[36] = {
            // ---- Positions ----     ---- Colors ----        ---- Normals ----
            -1.0f, 0.0f,  1.0f,       0.5f, 0.5f, 0.5f,       0.0f, 1.0f, 0.0f, // Front-Left
             1.0f, 0.0f,  1.0f,       0.5f, 0.5f, 0.5f,       0.0f, 1.0f, 0.0f, // Front-Right
             1.0f, 0.0f, -1.0f,       0.5f, 0.5f, 0.5f,       0.0f, 1.0f, 0.0f, // Back-Right
            -1.0f, 0.0f, -1.0f,       0.5f, 0.5f, 0.5f,       0.0f, 1.0f, 0.0f  // Back-Left
        };

        unsigned int indices[6] = {
            0, 1, 2,
            0, 2, 3
        };

        const float* getVertices() const override { return vertices; }
        const unsigned int getVertexSize() const override { return sizeof(vertices); }
        const unsigned int* getIndices() const override { return indices; }
        const unsigned int getIndexCount() const override { return sizeof(indices) / sizeof(unsigned int); }
    };

    class ShapeLibrary
    {
    public:
        static std::vector<std::unique_ptr<Shape>> CreateDefaultShapes()
        {
            std::vector<std::unique_ptr<Shape>> shapes;
            shapes.push_back(std::make_unique<CubeShape>());
            shapes.push_back(std::make_unique<PyramidShape>());
            shapes.push_back(std::make_unique<PlaneShape>());
            return shapes;
        }
    };

    struct Vertex {
        glm::vec3 Position;  // Layout 0
        glm::vec3 Normal;    // Layout 1
        glm::vec2 TexCoords; // Layout 2
        glm::vec3 Color = glm::vec3(1.0f);    // Layout 3
    };

    // THE CONTAINER CLASS
    struct MeshData // Temporary Holds o Model data in CPU during loading it
    {
        std::vector<Vertex> Vertices;
        std::vector<unsigned int> Indices;
    };

	class Mesh
	{
	private:
        std::shared_ptr<VertexArray> m_VAO;
        std::shared_ptr<VertexBuffer> m_VBO;
        std::shared_ptr<IndexBuffer> m_IBO;
        unsigned int m_IndexCount;
        // Material pointer owned by this specific mesh instance
        std::shared_ptr<Material> m_Material = nullptr;
	public:
        Mesh() = default;
        Mesh(const MeshData& data);

        // Material Getter & Setter
        void SetMaterial(const std::shared_ptr<Material>& material) { m_Material = material; }
        std::shared_ptr<Material> GetMaterial() const { return m_Material; }

		void Bind();
        inline unsigned int GetIndexCount() { return m_IndexCount; }
	};
}