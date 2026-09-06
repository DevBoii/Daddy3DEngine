#pragma once

#include "Engine/Renderer/Mesh.h"
#include "Engine/Renderer/Model.h"
#include "Engine/Renderer/Material.h"
#include <string>
#include <memory>

namespace DaddyEngine
{
    class MeshRenderer
    {
    public:
        // Direct reference to the specific mesh buffer to draw
        std::shared_ptr<Mesh> MeshAsset = nullptr;
        std::shared_ptr<Model> ModelAsset = nullptr;
        std::shared_ptr<Material> MaterialAsset = std::make_shared<Material>(); // Default material instance

        std::string ModelPath;

        bool IsVisible = true;
        bool CastShadow = true;

        MeshRenderer() = default;

        // Constructor for full Model loading
        MeshRenderer(const std::shared_ptr<Model>& model, const std::string& path = "")
            : ModelAsset(model), ModelPath(path)
        {
        }

        // Constructor for full Model with custom Material
        MeshRenderer(const std::shared_ptr<Model>& model, const std::shared_ptr<Material>& material, const std::string& path = "")
            : ModelAsset(model), MaterialAsset(material), ModelPath(path)
        {
        }

        // Constructor for individual Mesh slices instantiated from Model nodes
        MeshRenderer(const std::shared_ptr<Mesh>& mesh, const std::shared_ptr<Model>& model = nullptr, const std::shared_ptr<Material>& material = std::make_shared<Material>())
            : MeshAsset(mesh), ModelAsset(model), MaterialAsset(material)
        {
        }
    };
}