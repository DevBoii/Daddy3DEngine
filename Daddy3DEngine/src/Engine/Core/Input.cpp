#include "Input.h"
#include "Application.h"

namespace DaddyEngine
{
	bool Input::IsKeyPressed(int keyCode)
	{
		return (glfwGetKey(GetWindow(), keyCode) == GLFW_PRESS);
	}
	bool Input::IsKeyUp(int keyCode)
	{
		return (glfwGetKey(GetWindow(), keyCode) == GLFW_RELEASE);
	}

	bool Input::IsMouseButtonPressed(int button)
	{
		return (glfwGetMouseButton(GetWindow(), button) == GLFW_PRESS);
	}
	bool Input::IsMouseButtonUp(int button)
	{
		return (glfwGetMouseButton(GetWindow(), button) == GLFW_RELEASE);
	}

	void Input::EnableCursor()
	{
		glfwSetInputMode(GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	void Input::DisableCursor()
	{
		glfwSetInputMode(GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	double Input::GetMouseX()
	{
		double xPos;
		double yPos;
		glfwGetCursorPos(GetWindow(), &xPos, &yPos);
		return xPos;
	}
	double Input::GetMouseY()
	{
		double xPos;
		double yPos;
		glfwGetCursorPos(GetWindow(), &xPos, &yPos);
		return yPos;
	}

	GLFWwindow* Input::GetWindow()
	{
		return DaddyEngine::Application::Get().GetWindow();
	}
}
