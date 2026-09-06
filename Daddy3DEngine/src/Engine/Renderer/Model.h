#pragma once

#include "Mesh.h"
#include "Material.h"
#include "Texture.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <glm/glm.hpp>

namespace DaddyEngine
{
	struct ModelNode
	{
		std::string Name;
		glm::mat4 localTransform = glm::mat4(1.0f);
		std::vector<unsigned int> MeshesIndices;
		std::vector<ModelNode> Children;
	};

	class Model
	{
	private:
		std::string m_Directory;
		std::vector<std::shared_ptr<Mesh>> m_Meshes;
		// Store all materials parsed from the model file
		std::vector<std::shared_ptr<Material>> m_Materials;
		// Cache loaded textures by file path to prevent duplicate VRAM allocations
		std::unordered_map<std::string, std::shared_ptr<Texture2D>> m_LoadedTextures;
		ModelNode m_rootNode;

		void LoadModel(const std::filesystem::path& path);
		void ProcessNode(aiNode* node, const aiScene* scene, ModelNode& outNode);
		std::shared_ptr<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene);
		void ProcessMaterials(const aiScene* scene);
		std::shared_ptr<Texture2D> LoadMaterialTexture(aiMaterial* mat, aiTextureType type);

	public:
		Model() = default;
		Model(const std::filesystem::path& path);
		Model(const MeshData& data);

		const ModelNode& GetRootNode() const { return m_rootNode; }

		const std::vector<std::shared_ptr<Mesh>>& GetMeshes() const
		{
			return m_Meshes;
		}
		
		std::vector<std::shared_ptr<Mesh>>& GetMeshes()
		{
			return m_Meshes;
		}
	};
}