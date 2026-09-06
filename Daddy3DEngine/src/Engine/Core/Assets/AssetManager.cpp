#include "AssetManager.h"
#include <iostream>

namespace DaddyEngine
{
	void AssetManager::Init()
	{
		std::cout << "[AssetManager] Initializing Core Memory Repositories...\n";
	}

	void AssetManager::Shutdown()
	{
		std::cout << "[AssetManager] Clearing Cached Engine Assets Storage Pools...\n";
		s_ModelCache.clear();
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

	std::shared_ptr<Model> AssetManager::GetOrCreateModel(const std::string& assetPath)
	{
		// Already loaded?
		auto it = s_ModelCache.find(assetPath);

		if (it != s_ModelCache.end())
			return it->second;

		std::shared_ptr<Model> model;

		// Primitive
		if (assetPath == "Primitive::Cube")
		{
			model = std::make_shared<Model>(ConvertShapeToMeshData(CubeShape()));
		}
		else if (assetPath == "Primitive::Plane")
		{
			model = std::make_shared<Model>(ConvertShapeToMeshData(PlaneShape()));
		}
		else if (assetPath == "Primitive::Pyramid")
		{
			model = std::make_shared<Model>(ConvertShapeToMeshData(PyramidShape()));
		}
		else
		{
			// External model (.obj/.fbx/.gltf...)
			model = std::make_shared<Model>(assetPath);
		}

		s_ModelCache[assetPath] = model;

		return model;
	}
}