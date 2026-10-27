#include "Raycast.h"

namespace DaddyEngine
{
	//	Möller–Trumbore -> O + tD = uE1 ​+ vE2​
	bool Physics::Raycast::IntersectTriangle(Ray& ray, glm::vec3& A, glm::vec3& B, glm::vec3& C, float maxDistance)
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

		if (t < 0.0f || t > maxDistance)
			return false;

		ray.hitPoint = ray.origin + t * ray.direction;
		ray.normal = glm::normalize(glm::cross(edge1, edge2));
		ray.distance = t;

		return true;
	}

	bool Physics::Raycast::IntersectSquare(Ray& ray, glm::vec3& A, glm::vec3& B, glm::vec3& C, glm::vec3& D, float maxDistance)
	{
		glm::vec3 edge1 = B - A;
		glm::vec3 edge2 = D - A;

		glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

		float normalDotWithRay = glm::dot(normal, ray.direction);

		// Ray is Horizontal of plane
		if (std::abs(normalDotWithRay) < 0.000001f)
			return false;

		float t = glm::dot(normal, A - ray.origin) / normalDotWithRay;

		// Reject hits behind the ray or beyond the limit
		if (t < 0.0f || t > maxDistance)
			return false;

		glm::vec3 hitPoint = ray.origin + t * ray.direction;

		// Convert the hit point to square coordinates
		glm::vec3 toHit = hitPoint - A;

		float u = glm::dot(toHit, edge1) / glm::dot(edge1, edge1);
		float v = glm::dot(toHit, edge2) / glm::dot(edge2, edge2);

		// Check whether the point lies inside the square
		if (u < 0.0f || u > 1.0f || v < 0.0f || v > 1.0f)
			return false;

		ray.hitPoint = hitPoint;
		ray.normal = normal;
		ray.distance = t;

		return true;
	}
}