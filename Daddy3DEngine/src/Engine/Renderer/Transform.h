#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace DaddyEngine
{
	struct Transform
	{
		glm::vec3 Position = glm::vec3(0);
		glm::vec3 Rotation = glm::vec3(0);	// euler angle
		glm::vec3 Scale = glm::vec3(1.0f);

		glm::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.x), glm::vec3(1, 0, 0))
								* glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.y), glm::vec3(0, 1, 0))
								* glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.z), glm::vec3(0, 0, 1));

			return glm::translate(glm::mat4(1.0f), Position) * rotation * glm::scale(glm::mat4(1.0f), Scale);
		}

		glm::vec3 GetForward() const
		{
			return glm::vec3(0);
		}

		glm::vec3 GetRight() const
		{
			return glm::vec3(0);
		}

		glm::vec3 GetUp() const
		{
			return glm::vec3(0);
		}
	};
}