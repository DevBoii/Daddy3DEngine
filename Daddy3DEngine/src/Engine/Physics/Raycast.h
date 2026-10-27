#pragma once

#include "Engine/Maths/Ray.h"

namespace DaddyEngine
{
	namespace Physics
	{
		class Raycast
		{
		public:

			static bool IntersectTriangle(Ray& ray, glm::vec3& A, glm::vec3& B, glm::vec3& C, float maxDistance);
			static bool IntersectSquare(Ray& ray, glm::vec3& A, glm::vec3& B, glm::vec3& C, glm::vec3& D, float maxDistance);
		};
	}
}