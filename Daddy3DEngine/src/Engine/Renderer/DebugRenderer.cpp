#include "DebugRenderer.h"

#include <glad/glad.h>
#include <string>
#include "Shader.h"

namespace DaddyEngine
{
	DebugRenderer::DebugData DebugRenderer::s_Data;

	void DebugRenderer::Init()
	{
		std::string vertexSrc = R"(
			#version 330 core
			layout (location = 0) in vec3 aPos;
			layout (location = 1) in vec3 aColor;
			
			out vec3 vColor;
			uniform mat4 u_ViewProjection;

			void main()
			{
				vColor = aColor;
				gl_Position = u_ViewProjection * vec4(aPos, 1.0f);			
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core
			in vec3 vColor;
			
			out vec4 FragColor;

			void main()
			{
				FragColor = vec4(vColor, 1.0f);			
			}
		)";

		s_Data.LineShader = std::make_shared<Shader>(vertexSrc, fragmentSrc);

		// Generate IDs
		glGenVertexArrays(1, &s_Data.VAO);
		glGenBuffers(1, &s_Data.VBO);
		// Bind VAO then VBO
		glBindVertexArray(s_Data.VAO);
		glBindBuffer(GL_ARRAY_BUFFER, s_Data.VBO);
		// Upload Vertex Data
			// Allocate a Dynamic Buffer (e.g. Max 2000 lines = 4000 vertices)
			// GL_DYNAMIC_DRAW optimization for frequent CPU -> GPU updates
		const size_t maxVertices = 4000;
		glBufferData(GL_ARRAY_BUFFER, maxVertices * sizeof(LineVertex), nullptr, GL_DYNAMIC_DRAW);

		// Location 0: Position (vec3)
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (void*)offsetof(LineVertex, Position));
		// Location 1: Color (vec3)
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (void*)offsetof(LineVertex, Color));

		// Unbind buffer
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void DebugRenderer::DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color)
	{
		s_Data.LineVertices.push_back({ start, color });
		s_Data.LineVertices.push_back({ end, color });
	}

	void DebugRenderer::Flush(const glm::mat4& viewProjection, const float& lineThickness)
	{
		if (s_Data.LineVertices.empty()) return;

		s_Data.LineShader->Bind();
		s_Data.LineShader->SetUniformMat4("u_ViewProjection", viewProjection);

		glBindBuffer(GL_ARRAY_BUFFER, s_Data.VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, s_Data.LineVertices.size() * sizeof(LineVertex), s_Data.LineVertices.data());

		// Render all lines in 1 SINGLE Draw Call!
		glBindVertexArray(s_Data.VAO);

		// Optional: Line Thickness set karne ke liye
		glLineWidth(lineThickness);

		// Draw on top
		glDisable(GL_DEPTH_TEST);
		glDrawArrays(GL_LINES, 0, (GLsizei)s_Data.LineVertices.size());
		glEnable(GL_DEPTH_TEST);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		Clear();
	}

	void DebugRenderer::ShutDown()
	{
		glDeleteVertexArrays(1, &s_Data.VAO);
		glDeleteBuffers(1, &s_Data.VBO);
		s_Data.LineVertices.clear();
	}

	void DebugRenderer::Clear()
	{
		s_Data.LineVertices.clear();
	}

	void DebugRenderer::DrawTransform(const Transform& transform, const float length)
	{
		// Extract worldposition from matrix which is at 4th column
		glm::vec3 worldPosition = glm::vec3(transform.WorldMatrix[3]);
		
		// Extract normalized directional vectors directly out of the World Matrix columns
			// Column 0 = Right, Column 1 = Up, Column 2 = Forward
		glm::vec3 worldRight = transform.GetRight();
		glm::vec3 worldUp = transform.GetUp();
		glm::vec3 worldForward = transform.GetForward();

		// Forward
		DrawLine(worldPosition, worldPosition + (worldForward * length), glm::vec3(0.0f, 0.0f, 1.0f));
		// Up
		DrawLine(worldPosition, worldPosition + (worldUp * length), glm::vec3(0.0f, 1.0f, 0.0f));
		// Right
		DrawLine(worldPosition, worldPosition + (worldRight * length), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	void DebugRenderer::DrawDirectionalLight(const Transform& transform, const float length)
	{
		glm::vec3 pos = transform.GetWorldPosition();
		glm::vec3 dir = transform.GetForward();

		DrawLine(pos, pos + (dir * length), glm::vec3(1.0f, 1.0f, 0.0f));

		// Yellow Cross
		float gizmoSize = 0.2f;
		DrawLine(pos - glm::vec3(0, 0, gizmoSize) , pos + glm::vec3(0, 0, gizmoSize), glm::vec3(1.0f, 1.0f, 0.0f));
		DrawLine(pos - glm::vec3(0, gizmoSize, 0) , pos + glm::vec3(0, gizmoSize, 0), glm::vec3(1.0f, 1.0f, 0.0f));
		DrawLine(pos - glm::vec3(gizmoSize, 0, 0) , pos + glm::vec3(gizmoSize, 0, 0), glm::vec3(1.0f, 1.0f, 0.0f));
	}

	void DebugRenderer::DrawGrid(int sliceCount, float spacing, const glm::vec3& color)
	{
		float halfSize = (sliceCount * spacing) / 2.0f;

		for (int i = 0; i <= sliceCount; i++)
		{
			float pos = -halfSize + (i * spacing);

			// Z
			DrawLine(glm::vec3(pos, 0.0f, halfSize), glm::vec3(pos, 0.0f, -halfSize), color);
			// X
			DrawLine(glm::vec3(halfSize, 0.0f, pos), glm::vec3(-halfSize, 0.0f, pos), color);
		}
	}

	void DebugRenderer::DrawWorldAxes(float length)
	{
		// X Axis - Red
		DrawLine(glm::vec3(-length, 0.0f, 0.0f), glm::vec3(length, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		// Z Axis - Blue
		DrawLine(glm::vec3(0.0f, 0.0f, -length), glm::vec3(0.0f, 0.0f, length), glm::vec3(0.0f, 0.0f, 1.0f));
	}
}