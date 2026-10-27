#pragma once

#include <glm/glm.hpp>

namespace DaddyEngine
{
	struct Ray
	{
		glm::vec3 origin;
		glm::vec3 direction;
		glm::vec3 hitPoint;
		glm::vec3 normal;
		float distance;
	};
}