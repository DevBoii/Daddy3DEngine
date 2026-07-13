#include "Renderer.h"
#include <vector>
#include <algorithm>

namespace DaddyEngine
{
	std::vector<RendererQueue> Renderer::s_CommandQueue;
	glm::mat4 Renderer::s_ViewProjection = glm::mat4(1.0f);

	Renderer::Renderer()
	{

	}

	Renderer::~Renderer()
	{

	}

	void Renderer::Clear()
	{
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);	// greyish
		//glClearColor(0.5f, 0.0f, 0.0f, 1.0f);	// Red
		//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);	// Black
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Renderer::BeginScene(Camera& camera, bool& wireframe)
	{
		s_ViewProjection = camera.GetViewProjection();


		if (wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	void Renderer::Submit(Mesh& mesh, Shader& shader, const glm::mat4& modelMat4)
	{
		s_CommandQueue.push_back({ &mesh, &shader, modelMat4 });
	}

	// batch-optimized renderer
	void Renderer::EndScene()
	{
		//Sort the shader
		std::sort(s_CommandQueue.begin(), s_CommandQueue.end(), [](const RendererQueue& a, const RendererQueue& b) {
			return a.shader->GetShaderID() < b.shader->GetShaderID();
			});

		Shader* currentShader = nullptr;
		Mesh* currentMesh = nullptr;

		for (auto& cmd : s_CommandQueue)
		{
			if (currentShader != cmd.shader)
			{
				currentShader = cmd.shader;
				currentShader->Bind();
				currentShader->SetUniformMat4("u_ViewProjection", s_ViewProjection);
			}

			if (currentMesh != cmd.mesh) // Only rebind if it's a different mesh
			{
				currentMesh = cmd.mesh;
				currentMesh->Bind();
			}

			cmd.mesh->Bind();
			currentShader->SetUniformMat4("u_Model", cmd.transform);

			glDrawElements(GL_TRIANGLES, cmd.mesh->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
		}

		s_CommandQueue.clear();
	}
}