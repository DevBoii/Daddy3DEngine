#pragma once

#include "Engine/Maths/Ray.h"

namespace DaddyEngine
{
	class Raycast
	{
	public:

		static bool IntersectTriangle(Ray& ray, glm::vec3& A, glm::vec3& B, glm::vec3& C);
	};
}