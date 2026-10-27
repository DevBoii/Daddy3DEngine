#include "Application.h"
#include "Camera.h"
#include "KeyCodes.h"
#include "Input.h"
#include "Timestep.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/VertexBuffer.h"
#include "Engine/Renderer/VertexArray.h"
#include "Engine/Renderer/IndexBuffer.h"
#include "Engine/Renderer/Framebuffer.h"
#include "ImGui/ImGuiLayer.h"
#include "Engine/Scene/SceneManager.h"
#include "Sandbox/Scenes/SandboxScene.h" 
#include "Engine/Core/Assets/AssetManager.h"
#include "Engine/Editor/GizmoSettings.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace DaddyEngine
{
	Application* Application::s_Instance = nullptr;
	Camera* Application::m_EditorCamera = nullptr;
	GLFWwindow* Application::m_Window = nullptr;

	Application::Application()
	{
		s_Instance = this;
		std::cout << "Daddy's Engine is Started!" << std::endl;

		//Initializing GLFW
		if (!glfwInit()) {
			std::cout << "Could not initialize GLFW!" << std::endl;
		}
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		m_EditorCamera = new Camera(WIDTH, HEIGHT);
	}

	Application::~Application()
	{
		// Clear Scene allocation on close
		SceneManager::Shutdown();

		if (m_Window)
			glfwDestroyWindow(m_Window);

		glfwTerminate();
		std::cout << "Daddy's Engine is Stopped!" << std::endl;
	}

	void Application::run()
	{
		glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
		m_Window = glfwCreateWindow(WIDTH, HEIGHT, windowName, NULL, NULL);

		if (m_Window == NULL)
		{
			std::cout << "Failed to create GLFW window...!" << std::endl;
			glfwTerminate();
			return;
		}

		glfwMakeContextCurrent(m_Window);	// Set Window Current Context

		m_EditorCamera->SetViewportSize(GetWidth(), GetHeight());

		// Initialize GLAD for OPENGL 
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			return;
		}

		std::cout << "Daddy's Engine is Running..." << std::endl;

		// ==========================================
		// [SCENE RUNTIME INITIALIZATION]
		// Boot up your SandboxScene level now that the OpenGL context is fully ready!
		// ==========================================
		DaddyEngine::AssetManager::Init();

		Renderer renderer;
		renderer.Init();
		float m_LastFrameTime = 0.0f;

		SceneManager::SwitchScene(new Sandbox::SandboxScene());

		ImGuiLayer imguiLayer;
		imguiLayer.OnAttach();

		Framebuffer framebuffer;
		m_Framebuffer = &framebuffer;

		glEnable(GL_DEPTH_TEST);
		while (!glfwWindowShouldClose(m_Window) && !Input::IsKeyPressed(DADDY_KEY_ESCAPE))
		{
			float time = (float)glfwGetTime();
			// Create the Timestep (DeltaTime)
			Timestep ts = time - m_LastFrameTime;
			m_LastFrameTime = time;
			m_EditorCamera->DebugCameraController(ts);


			// Bind Framebuffer: Everything rendered now goes to a texture, not the screen
			m_Framebuffer->Bind();
			glViewport(0, 0, m_Framebuffer->width, m_Framebuffer->height);

			// ==========================================
			// [SCENE RUNTIME RENDERING]
			// Execute OnUpdate loop, which processes entity transforms 
			// and automatically triggers GetView<Transform, Mesh>() 
			// to Submit everything to your renderer pipeline!
			// ==========================================
			Scene* activeScene = SceneManager::GetActiveScene();
			if (activeScene)
			{
				// Clear scene using environment color
				renderer.Clear(activeScene->GetEnvironment().SkyColor);
				// Begin the camera pass context
				renderer.BeginScene(*m_EditorCamera, m_WireframeMode);
				// Update + submit scene objects
				activeScene->OnUpdate(ts, *GetEditorCamera());
				renderer.EndScene();
			}

			if(DaddyEngine::GizmoSettings::drawGizmos)
				DaddyEngine::DebugRenderer::Flush(m_EditorCamera->GetViewProjection());

			// Unbind: Stop rendering to the texture
			m_Framebuffer->Unbind();

			// Start ImGui
			renderer.Clear();
			imguiLayer.Begin(ts);

			// Ensure FILL mode for UI
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			// This will now use the texture from m_Framebuffer to draw the Scene window
			imguiLayer.OnImGuiRender(ts);

			imguiLayer.End();
			//std::cout << Input::GetMouseWorldPos().x << ", " << Input::GetMouseWorldPos().y << ", " << Input::GetMouseWorldPos().z << std::endl;

			glfwSwapBuffers(m_Window);
			glfwPollEvents();
		}
		imguiLayer.OnDetach();
		renderer.Shutdown();
		DaddyEngine::AssetManager::Shutdown();
	}

	void Application::quit()
	{
		glfwSetWindowShouldClose(m_Window, true);
	}

	int Application::GetWidth() {
		int w, h;
		glfwGetFramebufferSize(m_Window, &w, &h);
		return w;
	}

	int Application::GetHeight() {
		int w, h;
		glfwGetFramebufferSize(m_Window, &w, &h);
		return h;
	}
}