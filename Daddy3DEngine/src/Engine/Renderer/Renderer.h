#pragma once
#include "Mesh.h"
#include "Model.h"
#include "Material.h"
#include "Shader.h"
#include "Skybox.h"
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
		static std::unique_ptr<SkyboxRenderer> s_SkyboxRenderer;

	public:
		Renderer();
		~Renderer();
		void Init();
		void Shutdown();
		void BeginScene(Camera& camera, bool& wireframe);
		static void Submit(Mesh& mesh, std::shared_ptr<Material> material, Shader& shader, const glm::mat4& modelMat4);
		static void SubmitTerrain(Mesh& mesh, std::shared_ptr<Material> material, Shader& shader, const glm::mat4& modelMat4);
		static void Submit(Model& model, std::shared_ptr<Material> material, Shader& shader, const glm::mat4& modelMat4);
		void EndScene();
		void Clear(const glm::vec3& clearColor);
		void Clear();

		// Skybox Management
		static void RenderSkybox(Camera& camera, Environment& environment);
		static void SetSkybox(const std::string& path);
		static void RemoveSkybox();

		static uint32_t GetTriangleCount();
		static uint32_t GetDrawCalls();
	};
}