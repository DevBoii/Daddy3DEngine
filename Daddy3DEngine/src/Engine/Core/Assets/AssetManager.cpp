#include "AssetManager.h"
#include <iostream>

namespace DaddyEngine
{
	void AssetManager::Init()
	{
		std::cout << "[AssetManager] Initializing Core Memory Repositories...\n";

		s_MeshCache["Primitive::Cube"] = std::make_shared<Mesh>(ConvertShapeToMeshData(CubeShape()));
		s_MeshCache["Primitive::Pyramid"] = std::make_shared<Mesh>(ConvertShapeToMeshData(PyramidShape()));
		s_MeshCache["Primitive::Plane"] = std::make_shared<Mesh>(ConvertShapeToMeshData(PlaneShape()));

		std::cout << "[AssetManager] Static Primitives Cached Successfully!\n";
	}

	void AssetManager::Shutdown()
	{
		std::cout << "[AssetManager] Clearing Cached Engine Assets Storage Pools...\n";
		s_MeshCache.clear();
	}

	std::shared_ptr<Mesh> AssetManager::GetOrCreateMesh(const std::string& assetPath)
	{
		// Check 1: i already available
		auto it = s_MeshCache.find(assetPath);
		if (it != s_MeshCache.end())
		{
			return it->second;
		}

		// Check 2:  FUTURE 3D MODEL LOADER HOOK 
		//("Assets/Models/Car.obj"),
		if (assetPath.find("Primitive::") == std::string::npos)
		{
			std::cout << "[AssetManager] Streaming new external dynamic asset path: " << assetPath << "\n";
		}

		// Fallback safe reference trigger if asset invalid
		std::cout << "[AssetManager] Warning: Target reference path invalid! Fallback to standard Cube.\n";
		return s_MeshCache["Primitive::Cube"];
	}

	MeshData AssetManager::ConvertShapeToMeshData(const Shape& shape)
	{
		MeshData data;

		unsigned int m_IndexCount = shape.getIndexCount();
		const float* rawVertices = shape.getVertices();
		unsigned int totalFloats = shape.getVertexSize() / sizeof(float);

		// 24 vertices hain hamare pure layout array mein (6 faces * 4 corners)
		for (int i = 0; i < 24; i++)
		{
			int baseIndex = i * 9; // 9 floats ka hardcoded layout group jump
			Vertex v;

			// 1. Position Extractor (0, 1, 2)
			v.Position = glm::vec3(rawVertices[baseIndex + 0], rawVertices[baseIndex + 1], rawVertices[baseIndex + 2]);

			// 2. Normal Extractor (6, 7, 8) -> Humare positions ke baad colors hain, phir normals hain!
			v.Normal = glm::vec3(rawVertices[baseIndex + 6], rawVertices[baseIndex + 7], rawVertices[baseIndex + 8]);

			// 3. TexCoords Extractor (2 floats required)
			// Retro Mode optimization ke liye hum raw colors (indices 3, 4) ko hi dynamic UV spatial map frame bana rahe hain
			v.TexCoords = glm::vec2(0, 0);
			v.Color = glm::vec3(rawVertices[baseIndex + 3], rawVertices[baseIndex + 4], rawVertices[baseIndex + 5]);

			data.Vertices.push_back(v);
		}

		const unsigned int* rawIndices = shape.getIndices();
		// Indices array direct dump
		for (int i = 0; i < 36; i++)
		{
			data.Indices.push_back(rawIndices[i]);
		}

		return data;
	}
}