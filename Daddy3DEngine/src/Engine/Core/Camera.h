#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Engine/Maths/Ray.h"
#include "Timestep.h"

namespace DaddyEngine
{
	class Camera
	{
	private:

		glm::vec3 cameraPos = glm::vec3(0.0f, 5.0f, 7.0f);
		glm::vec3 camTarget = glm::vec3(0.0f, 0.0f, 0.0f);

		glm::vec3 camUp = glm::vec3(0.0f);
		glm::vec3 camRight = glm::vec3(0.0f);
		glm::vec3 camForward = glm::vec3(0.0f);

		glm::mat4 m_ViewProjection;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ProjectionMatrix;
		

		float m_Yaw = -90.0f;   // Horizontal rotation
		float m_Pitch = 0.0f;   // Vertical rotation
		float m_LastMouseX = 0.0f;
		float m_LastMouseY = 0.0f;
		bool m_FirstClick = true; // Prevents "camera snap" on first click

	public:
		int m_Width = 0;
		int m_Height = 0;
		Camera(int width = 1280, int height = 720, float camFOV = 45.0f);
		~Camera();

		glm::vec3 ScreenToWorldPoint(const glm::vec3& mouseScreenPosition);
		DaddyEngine::Ray ScreenToRay(const glm::vec3& mouseScreenPosition);

		void CalculateVP();
		void SetViewportSize(int width, int height);
		glm::mat4 GetViewProjection() const;
		glm::mat4 GetViewMatrix() const { return m_ViewMatrix; }
		glm::mat4 GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::vec3 GetPosition() const { return cameraPos; }
		const glm::vec3 GetForward() const { return camForward; }
		const glm::vec3 GetRight() const { return camRight; }
		const glm::vec3 GetUpn() const { return camUp; }

		void DebugCameraController(Timestep ts);

		float m_FOV;
		float cameraSpeed = 5;
		float sensitivity = 0.1f;
		float m_nearPlane = 0.1f;
		float m_farPlane = 100.0f;
	};
}