#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace DaddyEngine
{
	struct Transform
	{
		// Local space relative to parent (or world origin if root)
		glm::vec3 Position = glm::vec3(0.0f);
		glm::vec3 Rotation = glm::vec3(0.0f); // Euler angles (degrees)
		glm::vec3 Scale = glm::vec3(1.0f);

		// Computed world-space matrix (Updated by Scene hierarchy pass)
		glm::mat4 WorldMatrix = glm::mat4(1.0f);

		glm::mat4 GetLocalTransform() const
		{
			glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.x), glm::vec3(1, 0, 0))
				* glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.y), glm::vec3(0, 1, 0))
				* glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.z), glm::vec3(0, 0, 1));

			return glm::translate(glm::mat4(1.0f), Position) * rotation * glm::scale(glm::mat4(1.0f), Scale);
		}

		glm::mat4 GetTransform() const
		{
			return WorldMatrix;
		}

		glm::vec3 GetWorldPosition() const
		{
			return glm::vec3(WorldMatrix[3]);
		}

		// Column 2 tracks Forward (OpenGL right-hand space: -Z)
		glm::vec3 GetForward() const
		{
				// glm::normalize(-glm::vec3(WorldMatrix[2])); Right handed
			// Left-Handed like Unity
			return glm::normalize(glm::vec3(WorldMatrix[2]));
		}

		// Column 0 tracks Right (+X)
		glm::vec3 GetRight() const
		{
			return glm::normalize(glm::vec3(WorldMatrix[0]));
		}

		// Column 1 tracks Up (+Y)
		glm::vec3 GetUp() const
		{
			return glm::normalize(glm::vec3(WorldMatrix[1]));
		}
	};
}