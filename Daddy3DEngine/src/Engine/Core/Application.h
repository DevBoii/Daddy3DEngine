#pragma once
#include <iostream>

#define VERSION "Version 1.0"

struct GLFWwindow;

namespace DaddyEngine
{
	class Camera;
	class Framebuffer;

	class Application
	{
	public:

		int WIDTH = 1280;
		int HEIGHT = 720;
		bool m_WireframeMode = false;

		std::string shaderSource = "F:\\C++Codes\\OpenGL\\Daddy3DEngine\\Daddy3DEngine\\assets\\shaders\\basic.glsl";

		const char* windowName = "Daddy's Engine! " VERSION;

		Application();
		~Application();

		void run();
		void quit();

		bool m_running = false;

		static Application& Get() { return *s_Instance; }	// Singleton
		static GLFWwindow* GetWindow() { return m_Window; }
		static Camera* GetEditorCamera() { return m_EditorCamera; }
		int GetWidth() const { return WIDTH; }
		int GetHeight() const { return HEIGHT; }
		Framebuffer* GetFramebuffer() const { return m_Framebuffer; }

		int GetWidth();
		int GetHeight();

	private:
		static Application* s_Instance;
		static GLFWwindow* m_Window;
		static Camera* m_EditorCamera;
		Framebuffer* m_Framebuffer;
	};
}