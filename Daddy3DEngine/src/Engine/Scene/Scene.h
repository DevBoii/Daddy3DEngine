#pragma once
#include <vector>
#include <string>

#include "Engine/Core/ECS/Registry.h"  
#include "Engine/Core/Timestep.h"  
#include "Engine/Renderer/Shader.h"
#include "Engine/Core/Camera.h"
#include "Engine/Renderer/Model.h"
#include "Engine/Scene/Environment/Environment.h"

namespace DaddyEngine
{
	class Scene
	{
	private:
		Registry m_Registry;
		std::string m_SceneName;
		Shader m_BasicShader;
		
		Environment m_Environment;

		void CreateDirectionalLightEntity();

	public:

		Scene(const std::string& sceneName = "default") : m_SceneName(sceneName), m_BasicShader("Daddy3DEngine/assets/shaders/basic.glsl")
		{
		}
		virtual ~Scene() = default; // Crucial for virtual classes!

		virtual void OnCreate();
		virtual void OnUpdate(Timestep s, Camera& camera);
		virtual void OnDestroy();

		const std::string GetName() { return m_SceneName; }

		uint32_t CreateEntity(const std::string& name = "Entity");
		void DestroyEntity(uint32_t entity);

		void SetParent(uint32_t childEntity, uint32_t newParentEntity);
		bool IsDescendant(uint32_t possibleAncestorID, uint32_t entityID);
		void UpdateSceneTransforms();
		void UpdateEntityTransform(uint32_t entityID, const glm::mat4& parentWorldMatrix);
		uint32_t InstantiateModel(const std::shared_ptr<Model>& model, const std::string& name);
		uint32_t InstantiateNode(const ModelNode& node, const std::shared_ptr<Model>& model, const uint32_t parentEntity);

		void OnRender(Camera& camera); // Keep this non-virtual if rendering is the same for all scenes

		Registry& GetRegistry() { return m_Registry; }

		void SetSkybox(const std::string& path);

		Environment& GetEnvironment() { return m_Environment; }
		const Environment& GetEnvironment() const { return m_Environment; }
	};
}