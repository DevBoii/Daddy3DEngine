#include "Renderer.h"
#include <vector>
#include <algorithm>

namespace DaddyEngine
{
	std::vector<RendererQueue> Renderer::s_CommandQueue;
	glm::mat4 Renderer::s_ViewProjection = glm::mat4(1.0f);
	uint32_t Renderer::s_TriangleCount = 0;
	uint32_t Renderer::s_DrawCalls = 0;
	std::unique_ptr<SkyboxRenderer> Renderer::s_SkyboxRenderer = nullptr;

	uint32_t Renderer::GetTriangleCount()
	{
		return s_TriangleCount;
	}
	uint32_t Renderer::GetDrawCalls()
	{
		return s_DrawCalls;
	}

	Renderer::Renderer()
	{

	}

	Renderer::~Renderer()
	{

	}

	void Renderer::Init()
	{
		s_SkyboxRenderer = std::make_unique<SkyboxRenderer>();
	}

	void Renderer::Shutdown()
	{
		s_SkyboxRenderer.reset();
	}

	void Renderer::Clear()
	{
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Renderer::Clear(const glm::vec3& clearColor)
	{
		glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0f);
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

	void Renderer::Submit(Mesh& mesh, std::shared_ptr<Material> material, Shader& shader, const glm::mat4& modelMat4)
	{
		s_CommandQueue.push_back({ &mesh, &shader, modelMat4, material });
	}

	void Renderer::Submit(Model& model, std::shared_ptr<Material> material, Shader& shader, const glm::mat4& modelMat4)
	{
		// model.GetMeshes() returns std::vector<std::shared_ptr<Mesh>>
		for (const auto& meshPtr : model.GetMeshes())
		{
			if (meshPtr)
			{
				Submit(*meshPtr, material, shader, modelMat4);
			}
		}
	}

	void Renderer::RenderSkybox(Camera& camera)
	{
		if (s_SkyboxRenderer)
		{
			s_SkyboxRenderer->RenderSkyBox(camera);
		}
	}

	void Renderer::SetSkybox(const std::string& path)
	{
		if (!s_SkyboxRenderer)
			return;

		auto skybox = std::make_shared<Skybox>(path);

		s_SkyboxRenderer->SetSkybox(skybox);
	}

	void Renderer:: RemoveSkybox()
	{
		if (s_SkyboxRenderer)
		{
			s_SkyboxRenderer->RemoveSkybox();
		}
	}

	// batch-optimized renderer
	void Renderer::EndScene()
	{
		s_TriangleCount = 0;

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

			// Bind this entity's material to the shader!
			if (cmd.material)
			{
				cmd.material->Bind(*currentShader);
			}

			if (currentMesh != cmd.mesh) // Only rebind if it's a different mesh
			{
				currentMesh = cmd.mesh;
				currentMesh->Bind();
			}

			currentShader->SetUniformMat4("u_Model", cmd.transform);
			glDrawElements(GL_TRIANGLES, cmd.mesh->GetIndexCount(), GL_UNSIGNED_INT, nullptr);

			s_DrawCalls++;
			// Count triangles submitted
			s_TriangleCount += cmd.mesh->GetIndexCount() / 3;
		}

		s_CommandQueue.clear();
	}
}