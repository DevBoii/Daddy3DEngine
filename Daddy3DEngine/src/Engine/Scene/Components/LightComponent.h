#pragma once

#include <glm/glm.hpp>

namespace DaddyEngine
{
	class LightComponent
	{
	public:
		enum class LightType { Directional = 0, Point = 1, Spot = 2 };

		LightType Type = LightType::Directional;
		bool isActive = true;								// Toggle
		glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f); // Pure color tint
		float intensity = 1.0f;                             // Light multiplier / brightness control
		float ambientStrength = 0.3f;	                    // 30% base ambient factor
	};
}