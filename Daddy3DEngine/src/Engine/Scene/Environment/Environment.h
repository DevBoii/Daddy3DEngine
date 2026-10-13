#pragma once

namespace DaddyEngine
{
	enum class SkyType
	{
		SolidColor,
		Skybox
	};

	struct FogSettings
	{
		bool useFog = false;
		bool useSkyFog = false;
		glm::vec3 fogColor = glm::vec3(1);
		glm::vec3 skyFogColor = glm::vec3(1);
		bool useSameColor = true;
		float fogDensity = 0.1f;
		float skyFogIntensity = 0.1f;
	};

	struct Environment
	{
		SkyType SkyType = SkyType::Skybox;
		std::string SkyboxPath;
		glm::vec3 SkyColor{ 0.0f, 0.0f, 0.0f };

		FogSettings m_FogSettings;
	};
}