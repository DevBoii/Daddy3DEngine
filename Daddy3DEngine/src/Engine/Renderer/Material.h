#pragma once

#include "Texture.h"
#include "Shader.h"
#include <memory>

namespace DaddyEngine
{
	class Material
	{
	public:
		// Base color
		glm::vec3 AlbedoColor = glm::vec3(0.8f, 0.8f, 0.8f);
		float SpecularIntensity = 0.5f;
		float Shininess = 32.0f;

		std::shared_ptr<Texture2D> AlbedoMap = nullptr;
		std::shared_ptr<Texture2D> SpecularMap = nullptr;

		Material() = default;
		Material(const glm::vec3& color) : AlbedoColor(color) {}
		// Copy Constructor: Copies values & texture pointers, NOT heavy GPU VRAM
		Material(const Material& other)
			: AlbedoColor(other.AlbedoColor),
			SpecularIntensity(other.SpecularIntensity),
			Shininess(other.Shininess),
			AlbedoMap(other.AlbedoMap),
			SpecularMap(other.SpecularMap)
		{}

		void Bind(Shader& shader);
	};
}