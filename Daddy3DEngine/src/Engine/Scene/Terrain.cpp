#include "Terrain.h"
#include "Engine/Renderer/Material.h"
#include "Engine/Renderer/DebugRenderer.h"
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include "Engine/Maths/Ray.h"
#include "Engine//Core/Input.h"
#include "Engine/Core/Application.h"
#include "Engine/Core/Camera.h"
#include "Engine/Physics/Raycast.h"
#include "Engine/Core/ImGui/ImGuiLayer.h"

namespace DaddyEngine
{
	Terrain::Terrain(int Width, int Height)
		:width(Width), height(Height)
	{
		GenerateTerrain();
	}

	Terrain::~Terrain()
	{

	}

	void Terrain::GenerateTerrain()
	{
		m_Heightmap.assign(width * height, 0.0f);

		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		// Vertices
		for (int z = 0; z < height; z++)
		{
			for (int x = 0; x < width; x++)
			{
				// TOP-LEFT is starting of terrain
				Vertex vertex;

				int index = GetIndex(x, z);

				float xPos = spacing * x;
				float zPos = spacing * z;
				float yPos = m_Heightmap[index];
				vertex.Position = glm::vec3(xPos, yPos, zPos);
				vertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
				vertex.Color = glm::vec3(1.0f);

				// UV Coords ( value bw 0 to 1 for whole terrain)
				float u = static_cast<float>(x) / (width - 1);
				float v = static_cast<float>(z) / (height - 1);
				vertex.TexCoords = glm::vec2(u, v);

				vertices.push_back(vertex);
			}
		}

		// Indices
		for (int z = 0; z < height - 1; z++)
		{
			for (int x = 0; x < width - 1; x++)
			{
				// A quad
				unsigned int A = z * width + x;
				unsigned int B = A + 1;
				unsigned int C = (z + 1) * width + x;
				unsigned int D = C + 1;

				// Triangle 1
				indices.push_back(A);
				indices.push_back(C);
				indices.push_back(B);

				// Traingle 2
				indices.push_back(C);
				indices.push_back(D);
				indices.push_back(B);
			}
		}

		MeshData meshData;
		meshData.Vertices = vertices;
		meshData.Indices = indices;

		// Instantiates Mesh with GL_DYNAMIC_DRAW
		m_Mesh = std::make_shared<Mesh>(meshData, false);

		DebugTerrainColor();
	}

	void Terrain::Sculpt(const glm::vec3& worldHitPos, const float radius, const float intensity, const bool smoothFalloff)
	{
		if (!m_Mesh) return;

		auto& vertices = m_Mesh->GetVertices();
		bool modified = false;

		const float radiusSquared = radius * radius;

		// Get the vertices within brush range
		for (auto& vertex : vertices)
		{
			// Distance in the XZ plane
			float squaredDistance = glm::distance2(glm::vec2(vertex.Position.x, vertex.Position.z), glm::vec2(worldHitPos.x, worldHitPos.z));

			if (squaredDistance < radiusSquared)
			{
				if (smoothFalloff)
				{
					float distance = std::sqrt(squaredDistance);
					// Cosine falloff for smooth organic mound shape
					float falloff = 0.5f * (1.0f + glm::cos((distance / radius) * glm::pi<float>()));
					vertex.Position.y += intensity * falloff;
				}
				else
				{
					vertex.Position.y += intensity;
				}
				modified = true;
			}
		}

		if (modified)
		{
			RecalculateNormals();
			m_Mesh->UpdateVertices(vertices);
		}
	}

	void Terrain::RecalculateNormals()
	{
		auto& vertices = m_Mesh->GetVertices();

		// Reset all vertex normals
		for (auto& vertex : vertices)
		{
			vertex.Normal = glm::vec3(0.0f);
		}

		for (int z = 0; z < height - 1; z++)
		{
			for (int x = 0; x < width - 1; x++)
			{
				unsigned int A = z * width + x;
				unsigned int B = A + 1;
				unsigned int C = (z + 1) * width + x;
				unsigned int D = C + 1;

				// For Triangle1 (A, C, B)
				glm::vec3 normal1 = glm::cross(vertices[C].Position - vertices[A].Position, 
												vertices[B].Position - vertices[A].Position);

				vertices[A].Normal += normal1;
				vertices[C].Normal += normal1;
				vertices[B].Normal += normal1;

				// Triangle 2 (C, D, B)
				glm::vec3 normal2 = glm::cross(vertices[D].Position - vertices[C].Position,
												vertices[B].Position - vertices[C].Position);
				vertices[C].Normal += normal2;
				vertices[D].Normal += normal2;
				vertices[B].Normal += normal2;
			}
		}

		// Normalize final vectors
		for (auto& vertex : vertices)
		{
			vertex.Normal = glm::normalize(vertex.Normal);
		}
	}

	void Terrain::ApplyBrush()
	{
		switch (brush.mode)
		{
		case Brush::Mode::SMOOTH_RAISE:
			Sculpt(brush.position, brush.radius,
				brush.intensity, true);
			break;

		case Brush::Mode::HARD_RAISE:
			Sculpt(brush.position, brush.radius,
				brush.intensity, false);
			break;

		case Brush::Mode::SMOOTH_LOWER:
			Sculpt(brush.position, brush.radius,
				-brush.intensity, true);
			break;

		case Brush::Mode::HARD_LOWER:
			Sculpt(brush.position, brush.radius,
				-brush.intensity, false);
			break;

		case Brush::Mode::NONE:
			break;
		}
	}

	void Terrain::EditTerrain()
	{
		if (!ImGuiLayer::g_MouseInsideScene)
			return;

		auto* camera = Application::Get().GetEditorCamera();

		glm::vec2 sceneMouse = ImGuiLayer::g_SceneMousePosition;

		Ray ray = camera->ScreenToRay(glm::vec3(sceneMouse.x, sceneMouse.y, 0.0f));

		// Build this using your terrain's actual position,
		// rotation and scale.

		glm::mat4 inverseModel = glm::inverse(modelMatrix);

		// Convert the world-space ray into local space.
		glm::vec3 localOrigin = glm::vec3(inverseModel * glm::vec4(ray.origin, 1.0f));

		glm::vec3 localDirection = glm::normalize(glm::vec3(inverseModel * glm::vec4(ray.direction, 0.0f)));

		Ray localRay(localOrigin, localDirection);

		// Terrain bounds in LOCAL space.
		float maxX = spacing * (width - 1);
		float maxZ = spacing * (height - 1);

		glm::vec3 A(0.0f, 0.0f, 0.0f);
		glm::vec3 B(maxX, 0.0f, 0.0f);
		glm::vec3 C(0.0f, 0.0f, maxZ);
		glm::vec3 D(maxX, 0.0f, maxZ);

		if (Physics::Raycast::IntersectSquare(localRay, A, B, C, D, 10000.0f))
		{
			brush.position = glm::vec3(localRay.hitPoint);

			if (Input::IsMouseButtonPressed(DADDY_MOUSE_BUTTON_LEFT))
				ApplyBrush();
		}
	}

	void Terrain::DrawNormals()
	{
		if (!m_Mesh || !drawNormals)
			return;

		auto& vertices = m_Mesh->GetVertices();

		const int debugStep = 4;

		for (int z = 0; z < height; z += debugStep)
		{
			for (int x = 0; x < width; x += debugStep)
			{
				int index = z * width + x;

				auto& vertex = vertices[index];

				DebugRenderer::DrawLine(
					vertex.Position,
					vertex.Position + vertex.Normal * 0.2f,
					glm::vec3(0.0f, 0.6f, 0.0f)
				);
			}
		}
	}

	void Terrain::DebugTerrainColor()
	{
		auto mat = std::make_shared<Material>();
		mat->AlbedoColor = glm::vec3(0.85f, 0.85f, 0.85f);
		m_Mesh->SetMaterial(mat);
	}
}