#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace DaddyEngine
{
	class Input
	{
	public:

		static bool IsKeyPressed(int keyCode);
		static bool IsKeyUp(int keyCode);
		static bool IsMouseButtonPressed(int button);
		static bool IsMouseButtonUp(int button);

		static double GetMouseX();
		static double GetMouseY();
		static void EnableCursor();
		static void DisableCursor();
	private:
		static GLFWwindow* GetWindow();
	};
}