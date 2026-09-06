#include "Model.h"
#include "Engine/Editor/ConsolePanel.h"

namespace DaddyEngine
{
	Model::Model(const std::filesystem::path& path)
	{
		LoadModel(path);
	}

	Model::Model(const MeshData& data)
	{
		m_Meshes.push_back(std::make_shared<DaddyEngine::Mesh>(data));
	}

	void Model::LoadModel(const std::filesystem::path& path)
	{
		// Extract base directory for relative texture path resolution
		m_Directory = path.parent_path().string();
		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(path.string(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::string error = "[DaddyEngine] Assimp Error: " + std::string(importer.GetErrorString());
			//std::cerr << error << std::endl;
			ConsolePanel::InsertError(error);
			return;
		}

		// 1. Process all materials defined in the scene file
		ProcessMaterials(scene);

		// 2. Process node hierarchy and meshes
		ProcessNode(scene->mRootNode, scene, m_rootNode);
	}

	void Model::ProcessNode(aiNode* node, const aiScene* scene, ModelNode& outNode)
	{
		outNode.Name = node->mName.C_Str();

		/// Convert Assimp row-major transformation matrix to GLM column-major matrix
		aiMatrix4x4 t = node->mTransformation;
		outNode.localTransform = glm::mat4(
			t.a1, t.b1, t.c1, t.d1,
			t.a2, t.b2, t.c2, t.d2,
			t.a3, t.b3, t.c3, t.d3,
			t.a4, t.b4, t.c4, t.d4
		);

		// Process all meshes attached to this node
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			unsigned int meshIndex = node->mMeshes[i];

			// Only process mesh if m_Meshes hasn't allocated this index yet
			if (meshIndex >= m_Meshes.size())
			{
				m_Meshes.resize(meshIndex + 1);
			}

			if (!m_Meshes[meshIndex] && meshIndex < scene->mNumMeshes)
			{
				m_Meshes[meshIndex] = ProcessMesh(scene->mMeshes[meshIndex], scene);
			}

			outNode.MeshesIndices.push_back(meshIndex);
		}
		// Recursively process child nodes
		outNode.Children.resize(node->mNumChildren);
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			ProcessNode(node->mChildren[i], scene, outNode.Children[i]);
		}
	}

	std::shared_ptr<Mesh> Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
    {
        DaddyEngine::MeshData data;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            DaddyEngine::Vertex vertex;

            // Position
            vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };

            // Normal
            if (mesh->HasNormals())
                vertex.Normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
            else
                vertex.Normal = glm::vec3(0.0f);

            // UV Coordinates
            if (mesh->mTextureCoords[0])
            {
                vertex.TexCoords = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
            }
            else
            {
                vertex.TexCoords = glm::vec2(0.0f);
            }

            data.Vertices.push_back(vertex);
        }

        // Indices
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                data.Indices.push_back(face.mIndices[j]);
            }
        }

		// Create Mesh instance
		auto meshPtr = std::make_shared<Mesh>(data);

		// Assign parsed material to the mesh (if valid material index)
		if (mesh->mMaterialIndex < m_Materials.size())
		{
			meshPtr->SetMaterial(m_Materials[mesh->mMaterialIndex]);
		}

		return meshPtr;
    }

	void Model::ProcessMaterials(const aiScene* scene)
	{
		m_Materials.reserve(scene->mNumMaterials);

		for (unsigned int i = 0; i < scene->mNumMaterials; i++)
		{
			aiMaterial* aiMat = scene->mMaterials[i];
			auto engineMaterial = std::make_shared<Material>();

			// Extract AlbedoColor
			aiColor3D color(1.0f, 1.0f, 1.0f);
			if (aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
			{
				engineMaterial->AlbedoColor = glm::vec3(color.r, color.g, color.b);
			}

			// Extract Albedo / Diffuse Map
			if (aiMat->GetTextureCount(aiTextureType_DIFFUSE) > 0)
			{
				engineMaterial->AlbedoMap = LoadMaterialTexture(aiMat, aiTextureType_DIFFUSE);
			}
			// Extract Specular Map (if available)
			if (aiMat->GetTextureCount(aiTextureType_SPECULAR) > 0)
			{
				engineMaterial->AlbedoMap = LoadMaterialTexture(aiMat, aiTextureType_SPECULAR);
			}
			m_Materials.push_back(engineMaterial);
		}
	}

	std::shared_ptr<Texture2D> Model::LoadMaterialTexture(aiMaterial* mat, aiTextureType type)
	{
		aiString str;
		mat->GetTexture(type, 0, &str);

		std::string rawPath = str.C_Str();

		// 1. Remove leading slashes/backslashes to ensure relative path resolution
		while (!rawPath.empty() && (rawPath.front() == '/' || rawPath.front() == '\\'))
		{
			rawPath.erase(0, 1);
		}
		std::filesystem::path texturePath = std::filesystem::path(m_Directory) / rawPath;
		std::string pathKey = texturePath.string();

		//Check if texture already loaded
		auto it = m_LoadedTextures.find(pathKey);
		if (it != m_LoadedTextures.end())
		{
			return it->second; // return existing texture
		}

		// make new and add it if not already in
		auto texture = std::make_shared<Texture2D>(texturePath);
		m_LoadedTextures[pathKey] = texture;
		return texture;
	}
}