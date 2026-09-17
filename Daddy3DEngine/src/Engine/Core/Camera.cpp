#include "Camera.h"
#include "Input.h"
#include "KeyCodes.h"
#include "Engine/Core/ImGui/ImGuiLayer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"  

#include <iostream>

namespace DaddyEngine
{
	Camera::Camera(int width, int height, float camFOV)
	{
		m_FOV = camFOV;
		m_Width = width;
		m_Height = height;

		camForward = glm::normalize(camTarget - cameraPos);
		camRight = glm::normalize(glm::cross(camForward, glm::vec3(0.0f, 1.0f, 0.0f)));
		camUp = glm::normalize(glm::cross(camRight, camForward));

		CalculateVP();
	}

	Camera::~Camera()
	{

	}

	void Camera::CalculateVP()
	{
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 proj = glm::mat4(1.0f);

		view = glm::lookAt(cameraPos, cameraPos + camForward, camUp);
		proj = glm::perspective(glm::radians(m_FOV), (float)m_Width / (float)m_Height, m_nearPlane, m_farPlane);

        m_ViewMatrix = view;
        m_ProjectionMatrix = proj;
		m_ViewProjection = proj * view;
	}

	glm::mat4 Camera::GetViewProjection() const
	{
		return m_ViewProjection;
	}

	void Camera::SetViewportSize(int width, int height)
	{
		m_Width = width;
		m_Height = height;
		CalculateVP();
	}

    void Camera::DebugCameraController(Timestep ts)
    {
        // 1. Block input if mouse is not over scene view
        if (ImGuiLayer::g_SceneWindowHovered | !m_FirstClick)
        {
            // 2. Handle Rotation (Right Click Held)
            if (DaddyEngine::Input::IsMouseButtonPressed(DADDY_MOUSE_BUTTON_RIGHT))
            {
                // Hide cursor like Unity when flying
                DaddyEngine::Input::DisableCursor();

                float mouseX = DaddyEngine::Input::GetMouseX();
                float mouseY = DaddyEngine::Input::GetMouseY();

                if (m_FirstClick) {
                    m_LastMouseX = mouseX;
                    m_LastMouseY = mouseY;
                    m_FirstClick = false;
                }

                float offsetX = mouseX - m_LastMouseX;
                float offsetY = m_LastMouseY - mouseY; // Reversed: y-coords go from bottom to top
                m_LastMouseX = mouseX;
                m_LastMouseY = mouseY;

                m_Yaw += offsetX * sensitivity;
                m_Pitch += offsetY * sensitivity;

                // Constraints (Don't let the camera flip upside down)
                if (m_Pitch > 89.0f)  m_Pitch = 89.0f;
                if (m_Pitch < -89.0f) m_Pitch = -89.0f;

                // Calculate new Forward vector
                glm::vec3 front;
                front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
                front.y = sin(glm::radians(m_Pitch));
                front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));

                camForward = glm::normalize(front);
                // Recalculate Right and Up vectors
                camRight = glm::normalize(glm::cross(camForward, glm::vec3(0, 1, 0)));
                camUp = glm::normalize(glm::cross(camRight, camForward));

                float velocity = cameraSpeed * ts;
                if(DaddyEngine::Input::IsKeyPressed(DADDY_KEY_LEFT_SHIFT))
                    velocity = cameraSpeed * 2 * ts;

                if (DaddyEngine::Input::IsKeyPressed(DADDY_KEY_W)) cameraPos += camForward * velocity;
                if (DaddyEngine::Input::IsKeyPressed(DADDY_KEY_S)) cameraPos -= camForward * velocity;
                if (DaddyEngine::Input::IsKeyPressed(DADDY_KEY_A)) cameraPos -= camRight * velocity;
                if (DaddyEngine::Input::IsKeyPressed(DADDY_KEY_D)) cameraPos += camRight * velocity;
                if (DaddyEngine::Input::IsKeyPressed(DADDY_KEY_E)) cameraPos += camUp * velocity;
                if (DaddyEngine::Input::IsKeyPressed(DADDY_KEY_Q)) cameraPos -= camUp * velocity;

                CalculateVP();
            }
            else
            {
                m_FirstClick = true;
                DaddyEngine::Input::EnableCursor();
            }
        }
    }
}