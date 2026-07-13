#include "Scene.h"

// Include paths for components and engine pipelines
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Transform.h"
#include "Engine/Renderer/Mesh.h"      
#include "Engine/Renderer/Shader.h"
#include "Engine/Scene/Components/IdentityComponent.h"
#include "Engine/Scene/Components/MeshRenderer.h"

#include <iostream>

namespace DaddyEngine
{
	void Scene::OnRender()
	{
		Renderer renderer;
		renderer.Clear();

		auto renderView = m_Registry.GetView<Transform>();

		for (uint32_t entity : renderView)
		{
			if (m_Registry.HasComponent<MeshRenderer>(entity))
			{
				auto& transformComponent = m_Registry.GetComponent<Transform>(entity);
				auto& meshRenderer = m_Registry.GetComponent<MeshRenderer>(entity);

				// Render condition checks safety guards
				if (!meshRenderer.IsVisible || meshRenderer.MeshAsset == nullptr)
					continue;

				// Hum smart pointer ko de-reference (*) kar rahe hain kyunki aapka Submit raw reference (&) leta hai.
				renderer.Submit(*meshRenderer.MeshAsset, m_BasicShader, transformComponent.GetTransform());
			}
		}

		renderer.EndScene();
	}

	void Scene::OnCreate()
	{
		
	}

	void Scene::OnUpdate(Timestep s)
	{
		OnRender();
	}

	void Scene::OnDestroy()
	{

	}

	uint32_t Scene::CreateEntity(const std::string& name)
	{
		uint32_t entity = m_Registry.CreateEntity();

		m_Registry.AddComponent<IdentityComponent>(entity, IdentityComponent(name, "Default", 0));
		m_Registry.AddComponent<Transform>(entity, Transform());

		return entity;
	}

	void Scene::DestroyEntity(uint32_t entity)
	{
		m_Registry.DestroyEntity(entity);
	}
}