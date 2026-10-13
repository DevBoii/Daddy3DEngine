#include "Raycast.h"

namespace DaddyEngine
{
	//	Möller–Trumbore -> O + tD = uE1 ​+ vE2​
	bool Raycast::IntersectTriangle(Ray& ray, glm::vec3& A, glm::vec3& B, glm::vec3& C)
	{
		const float EPSILON = 0.0000001f;

		// vector from A To B and A to C
		glm::vec3 edge1 = (B - A);
		glm::vec3 edge2 = (C - A);
		glm::vec3 h = glm::cross(ray.direction, edge2);
		float a = glm::dot(edge1, h);

		// If 'a' is near zero, the ray is parallel to the triangle
		if (a > -EPSILON && a < EPSILON)
			return false;

		float f = 1.0f / a;
		// Vector from A to ray origin
		glm::vec3 s = ray.origin - A;

		// Position in the A -> B direction
		float u = f * glm::dot(s, h);

		if (u < 0.0f || u > 1.0f)
			return false;

		// Helper for calculating v
		glm::vec3 q = glm::cross(s, edge1);

		// Calculate v
		float v = f * glm::dot(ray.direction, q);

		if (v < 0.0f || u + v > 1.0f)
			return false;

		// Calculate distance along ray
		float t = f * glm::dot(edge2, q);

		if (t < 0.0f)
			return false;

		ray.hitPoint = ray.origin + t * ray.direction;
		ray.normal = glm::normalize(glm::cross(edge1, edge2));

		return true;
	}
}