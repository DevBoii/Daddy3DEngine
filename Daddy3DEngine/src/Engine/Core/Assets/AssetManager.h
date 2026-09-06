#pragma once

#include "Engine/Renderer/Mesh.h"
#include "Engine/Renderer/Model.h"
#include <unordered_map>
#include <string>
#include <memory>

namespace DaddyEngine
{
	class AssetManager
	{
	private:
		inline static std::unordered_map<std::string, std::shared_ptr<Model>> s_ModelCache;
		
		static MeshData ConvertShapeToMeshData(const Shape& shape);

	public:
		// Storage configuration setup initialization
		static void Init();

		// Clear all cached memory blocks on Engine shutdown
		static void Shutdown();

		static std::shared_ptr<Model> GetOrCreateModel(const std::string& assetPath);
	};
}
