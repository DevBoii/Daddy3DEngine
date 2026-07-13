#pragma once
#include <vector>
#include <string>

#include "Engine/Core/ECS/Registry.h"  
#include "Engine/Core/Timestep.h"  
#include "Engine/Renderer/Shader.h"

namespace DaddyEngine
{
	class Scene
	{
	private:
		Registry m_Registry;
		std::string m_SceneName;
		Shader m_BasicShader;

	public:

		Scene(const std::string& sceneName = "default") : m_SceneName(sceneName), m_BasicShader("F:\\C++Codes\\OpenGL\\Daddy3DEngine\\Daddy3DEngine\\assets\\shaders\\basic.glsl") { }
		virtual ~Scene() = default; // Crucial for virtual classes!

		virtual void OnCreate();
		virtual void OnUpdate(Timestep s);
		virtual void OnDestroy();

		const std::string GetName() { return m_SceneName; }

		uint32_t CreateEntity(const std::string& name = "Entity");
		void DestroyEntity(uint32_t entity);

		void OnRender(); // Keep this non-virtual if rendering is the same for all scenes

		Registry& GetRegistry() { return m_Registry; }
	};
}