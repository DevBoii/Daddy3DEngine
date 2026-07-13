#pragma once
#include "Mesh.h"
#include "Shader.h"
#include "Engine/Core/Camera.h"

namespace DaddyEngine
{
	struct RendererQueue
	{
		Mesh* mesh;
		Shader* shader;
		glm::mat4 transform;
	};

	class Renderer
	{
	private:
		static std::vector<RendererQueue> s_CommandQueue; // Must be static
		static glm::mat4 s_ViewProjection;

	public:
		Renderer();
		~Renderer();
		void BeginScene(Camera& camera, bool& wireframe);
		void Submit(Mesh& mesh, Shader& shader, const glm::mat4& modelMat4);
		void EndScene();
		void Clear();

	};
}