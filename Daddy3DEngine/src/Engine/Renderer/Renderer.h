#pragma once
#include "Mesh.h"
#include "Model.h"
#include "Material.h"
#include "Shader.h"
#include "Engine/Core/Camera.h"

namespace DaddyEngine
{
	struct RendererQueue
	{
		Mesh* mesh;
		Shader* shader;
		glm::mat4 transform;
		std::shared_ptr<Material> material;
	};

	class Renderer
	{
	private:
		static std::vector<RendererQueue> s_CommandQueue; // Must be static
		static glm::mat4 s_ViewProjection;
		static uint32_t s_TriangleCount;
		static uint32_t s_DrawCalls;

	public:
		Renderer();
		~Renderer();
		void BeginScene(Camera& camera, bool& wireframe);
		static void Submit(Mesh& mesh, std::shared_ptr<Material> material, Shader& shader, const glm::mat4& modelMat4);
		static void Submit(Model& model, std::shared_ptr<Material> material, Shader& shader, const glm::mat4& modelMat4);
		void EndScene();
		void Clear();

		static uint32_t GetTriangleCount();
		static uint32_t GetDrawCalls();
	};
}