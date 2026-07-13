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
		GLFWwindow* GetWindow() const { return m_Window; }
		inline int GetWidth() const { return WIDTH; }
		inline int GetHeight() const { return HEIGHT; }
		Camera* GetEditorCamera() const { return m_EditorCamera; }
		Framebuffer* GetFramebuffer() const { return m_Framebuffer; }

		int GetWidth();
		int GetHeight();

	private:
		static Application* s_Instance;
		GLFWwindow* m_Window;
		Camera* m_EditorCamera;
		Framebuffer* m_Framebuffer;
	};
}