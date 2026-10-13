#pragma once

#include "Engine/Renderer/Mesh.h"
#include "Engine/Renderer/Transform.h"
#include <memory>

namespace DaddyEngine
{
	struct Brush
	{
		enum class Mode
		{
			NONE,
			SMOOTH_RAISE,
			HARD_RAISE,
			SMOOTH_LOWER,
			HARD_LOWER
		};
		glm::vec3 position = glm::vec3(0.0f, 2.0f, 0.0f);
		float radius = 2.0f;
		float intensity = 0.1f;
		bool smoothFalloff = true;
		Mode mode = Mode::NONE;
	};

	class Terrain
	{	
	private:
		std::shared_ptr<Mesh> m_Mesh;
		void DebugTerrainColor();

	public:

		bool drawNormals = false;
		int width;
		int height;
		float spacing = 0.25f;

		Brush brush;

		Terrain(int Width = 32, int Height = 32);
		void GenerateTerrain();
		void Sculpt(const glm::vec3& worldHitPos, const float radius, const float intensity, const bool smoothFalloff = true);
		void ApplyBrush();
		void RecalculateNormals();
		void DrawNormals();
		std::shared_ptr<Mesh> GetMesh() { return m_Mesh;}
		~Terrain();
	};
}