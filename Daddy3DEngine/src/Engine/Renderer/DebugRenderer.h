#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <memory>

#include "Transform.h"

namespace DaddyEngine
{
	struct LineVertex
	{
		glm::vec3 Position;
		glm::vec3 Color;
	};

	class DebugRenderer
	{
	public:
		static void Init(); // To Intialize GPU Buffers

		// Submit commands
		static void DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color = glm::vec3(1.0f, 1.0f, 1.0f));

		// Render all submitted lines in 1 single draw call
		static void Flush(const glm::mat4& viewProjection, const float& lineThickness = 2.0f);

		static void ShutDown();
		static void Clear();

		static void DrawTransform(const Transform& transform, const float length = 1.0f);
		static void DrawDirectionalLight(const Transform& transform, const float length = 1.0f);
		static void DrawGrid(int sliceCount = 50, float spacing = 1.0f, const glm::vec3& color = glm::vec3(0.3f));
		static void DrawWorldAxes(float length = 25);

	private:
		struct DebugData
		{
			uint32_t VAO = 0;
			uint32_t VBO = 0;

			std::vector<LineVertex> LineVertices;
			std::shared_ptr<class Shader> LineShader;
		};

		static DebugData s_Data;
	};
}