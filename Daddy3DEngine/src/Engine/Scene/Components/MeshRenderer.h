#pragma once

#include "Engine/Renderer/Mesh.h"
#include <string>
#include <memory>

namespace DaddyEngine
{
	class MeshRenderer
	{
	private:

	public:
		std::shared_ptr<Mesh> MeshAsset = nullptr;
		std::string ModelPath = "Primitive::Cube";

		bool IsVisible = true;
		bool castShadow = true;

		MeshRenderer() = default;
		MeshRenderer(const std::shared_ptr<Mesh>& mesh, const std::string& path)
			:MeshAsset(mesh), ModelPath(path) { }
	};
}