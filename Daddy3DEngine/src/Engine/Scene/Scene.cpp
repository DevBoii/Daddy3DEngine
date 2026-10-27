#include "Scene.h"

// Include paths for components and engine pipelines
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Transform.h"
#include "Engine/Renderer/Mesh.h"      
#include "Engine/Renderer/Shader.h"
#include "Engine/Scene/Components/IdentityComponent.h"
#include "Engine/Scene/Components/RelationshipComponent.h"
#include "Engine/Scene/Components/MeshRenderer.h"
#include "Engine/Scene/Components/LightComponent.h"
#include "Engine/Scene/Terrain.h"
#include "Engine/Renderer/DebugRenderer.h"
#include "Engine/Editor/GizmoSettings.h"
#include <glm/gtx/matrix_decompose.hpp>

#include <iostream>

namespace DaddyEngine
{
	void Scene::OnRender(Camera& camera)
	{
		glm::vec3 finalLightColor = glm::vec3(1.0f, 1.0f, 1.0f);	// Default White
		glm::vec3 lightDirection = glm::vec3(1.0f, 1.0f, 1.0f);	
		float ambientStrength = 0.3f;						// 30 percent str
		float intensity = 1.0f;

		auto lightView = m_Registry.GetView<LightComponent>();
		for (uint32_t lightEntity : lightView)
		{
			auto& lightComponent = m_Registry.GetComponent<LightComponent>(lightEntity);
			if (lightComponent.isActive)
			{
				ambientStrength = lightComponent.ambientStrength;
				finalLightColor = lightComponent.lightColor;
				lightDirection = m_Registry.GetComponent<Transform>(lightEntity).GetForward();
				intensity = lightComponent.intensity;
				if(DaddyEngine::GizmoSettings::debugDirectionalLight)
					DaddyEngine::DebugRenderer::DrawDirectionalLight(m_Registry.GetComponent<Transform>(lightEntity), 2.0f);
				break; // found first light source
			}
		}
		//------------------------Light-------------------------------------
		m_BasicShader.Bind();
		m_BasicShader.SetUniformVec3("u_Light.u_LightColor", finalLightColor);
		m_BasicShader.SetUniformVec3("u_Light.u_LightDirection", lightDirection);
		m_BasicShader.SetUniform1f("u_Light.u_AmbientStrength", ambientStrength);
		m_BasicShader.SetUniform1f("u_Light.u_Intensity", intensity);
		//------------------------Fog---------------------------------------
		m_BasicShader.SetUniformVec3("u_CameraPosition", camera.GetPosition());
		m_BasicShader.SetUniform1i("u_Fog.useFog", m_Environment.m_FogSettings.useFog);
		m_BasicShader.SetUniformVec3("u_Fog.fogColor", m_Environment.m_FogSettings.fogColor);
		m_BasicShader.SetUniform1f("u_Fog.fogDensity", m_Environment.m_FogSettings.fogDensity);

		auto renderView = m_Registry.GetView<Transform>();

		for (uint32_t entity : renderView)
		{
			auto& transformComponent = m_Registry.GetComponent<Transform>(entity);
			if (m_Registry.HasComponent<MeshRenderer>(entity))
			{
				auto& meshRenderer = m_Registry.GetComponent<MeshRenderer>(entity);

				// Visibility check
				if (!meshRenderer.IsVisible)
					continue;

				// 1. Primary path: Render sub-mesh using calculated WorldMatrix
				if (meshRenderer.MeshAsset != nullptr)
				{
					Renderer::Submit(*meshRenderer.MeshAsset, meshRenderer.MaterialAsset, m_BasicShader, transformComponent.GetTransform());
				}
				// 2. Fallback path: Render entire model asset using calculated WorldMatrix
				else if (meshRenderer.ModelAsset != nullptr)
				{
					Renderer::Submit(*meshRenderer.ModelAsset, meshRenderer.MaterialAsset, m_BasicShader, transformComponent.GetTransform());
				}
			}

			if (m_Registry.HasComponent<Terrain>(entity))
			{
				auto& terrainComponent = m_Registry.GetComponent<Terrain>(entity);
				terrainComponent.position = m_Registry.GetComponent<Transform>(entity).Position;
				terrainComponent.modelMatrix = m_Registry.GetComponent<Transform>(entity).GetTransform();
				Renderer::SubmitTerrain(*terrainComponent.GetMesh(), terrainComponent.GetMesh()->GetMaterial(), m_BasicShader, transformComponent.GetTransform());
			}
		}

		//-----------Render Skybox
		if (m_Environment.SkyType == SkyType::Skybox)
			Renderer::RenderSkybox(camera, m_Environment);

		if (DaddyEngine::GizmoSettings::debugGrid)
			DaddyEngine::DebugRenderer::DrawGrid();
		if (DaddyEngine::GizmoSettings::debugWorldAxis)
			DaddyEngine::DebugRenderer::DrawWorldAxes();
	}

	void Scene::OnCreate()
	{
		CreateDirectionalLightEntity();
		
		SetSkybox("Assets/Skyboxes/sky_40_cubemap_2k/Default2.skybox");
	}

	void Scene::OnUpdate(Timestep s, Camera& camera)
	{
		// Recalculate all parent/child world matrices
		UpdateSceneTransforms();

		// Render pass uses updated WorldMatrix
		OnRender(camera);
	}

	void Scene::OnDestroy()
	{

	}

	void Scene::SetParent(uint32_t childEntity, uint32_t newParentEntity)
	{
		// Prevent self-parenting or circular loops
		if (newParentEntity != NullEntity && IsDescendant(childEntity, newParentEntity))
		{
			return; // Early return to abort invalid parenting
		}

		// First Check if children and parent have Relationship component
		if (!m_Registry.HasComponent<RelationshipComponent>(childEntity))
			m_Registry.AddComponent<RelationshipComponent>(childEntity, RelationshipComponent());
		// ONLY add RelationshipComponent to newParentEntity if it is NOT NullEntity!
		if (newParentEntity != NullEntity)
		{
			if (!m_Registry.HasComponent<RelationshipComponent>(newParentEntity))
				m_Registry.AddComponent<RelationshipComponent>(newParentEntity, RelationshipComponent());
		}

		// Preserve original world matrix before changing hierarchy
		glm::mat4 childWorldMatrix = glm::mat4(1.0f);
		if (m_Registry.HasComponent<Transform>(childEntity))
		{
			childWorldMatrix = m_Registry.GetComponent<Transform>(childEntity).WorldMatrix;
		}

		// 1. check if it has already a parent if yes then remove
		auto& childRel = m_Registry.GetComponent<RelationshipComponent>(childEntity);
		if (childRel.Parent != NullEntity && m_Registry.HasComponent<RelationshipComponent>(childRel.Parent))
		{
			auto& oldParentRL = m_Registry.GetComponent<RelationshipComponent>(childRel.Parent);
			std::erase(oldParentRL.Children, childEntity);	// C++20: Instantly removes matching values from the vector and shifts elements to close the gap.
		}

		// 2. Set new Parent
		childRel.Parent = newParentEntity;
		// Link into Parent Children list
		if (newParentEntity != NullEntity)
		{
			auto& newParentRL = m_Registry.GetComponent<RelationshipComponent>(newParentEntity);
			newParentRL.Children.push_back(childEntity);
		}

		// 3. Recalculate local transform relative to new parent so world position stays unchanged
		if (m_Registry.HasComponent<Transform>(childEntity))
		{
			auto& childTransform = m_Registry.GetComponent<Transform>(childEntity);

			glm::mat4 parentWorldMatrix = glm::mat4(1.0f);
			if (newParentEntity != NullEntity && m_Registry.HasComponent<Transform>(newParentEntity))
			{
				parentWorldMatrix = m_Registry.GetComponent<Transform>(newParentEntity).WorldMatrix;
			}

			// LocalMatrix = Inverse(ParentWorldMatrix) * ChildWorldMatrix
			glm::mat4 localMatrix = glm::inverse(parentWorldMatrix) * childWorldMatrix;

			// Decompose local matrix into local Position, Rotation, Scale
			glm::vec3 skew;
			glm::vec4 perspective;
			glm::quat orientation;

			glm::decompose(localMatrix, childTransform.Scale, orientation, childTransform.Position, skew, perspective);
			childTransform.Rotation = glm::degrees(glm::eulerAngles(orientation));
		}

		// Update scene graph immediately
		UpdateSceneTransforms();
	}

	void Scene::UpdateEntityTransform(uint32_t entityID, const glm::mat4& parentWorldMatrix)
	{
		if (!m_Registry.HasComponent<Transform>(entityID))
			return;

		auto& transform = m_Registry.GetComponent<Transform>(entityID);
		// World Matrix = Parent World Matrix * Local Matrix
		transform.WorldMatrix = parentWorldMatrix * transform.GetLocalTransform();
		// Traverse down into children if RelationshipComponent exists
		if (m_Registry.HasComponent<RelationshipComponent>(entityID))
		{
			auto& relationship = m_Registry.GetComponent<RelationshipComponent>(entityID);
			for (auto childID : relationship.Children)
				UpdateEntityTransform(childID, transform.WorldMatrix);
		}
	}

	uint32_t Scene::InstantiateModel(const std::shared_ptr<Model>& model, const std::string& name)
	{
		if (!model)
			return NullEntity;

		// 1. Root entity container for the entire model
		uint32_t rootEntity = CreateEntity(name);

		// 2. Build sub-entity tree starting from Assimp root node
		InstantiateNode(model->GetRootNode(), model, rootEntity);

		return rootEntity;
	}

	uint32_t Scene::InstantiateNode(const ModelNode& node, const std::shared_ptr<Model>& model, const uint32_t parentEntity)
	{
		uint32_t entity = CreateEntity(node.Name);
		SetParent(entity, parentEntity);

		// Decompose LocalTransform into local Position, Rotation, Scale components
		if (m_Registry.HasComponent<Transform>(entity))
		{
			auto& transform = m_Registry.GetComponent<Transform>(entity);

			glm::vec3 skew;
			glm::vec4 perspective;
			glm::quat orientation;
			glm::decompose(node.localTransform, transform.Scale, orientation, transform.Position, skew, perspective);
			transform.Rotation = glm::degrees(glm::eulerAngles(orientation));
		}

		// Attach mesh renderer to sub-entity if it owns a mesh
		if (!node.MeshesIndices.empty())
		{
			// Primary mesh bound directly to node entity using new MeshRenderer constructor
			unsigned int primaryMeshIndex = node.MeshesIndices[0];
			auto meshPtr = model->GetMeshes()[primaryMeshIndex];
			auto materialPtr = meshPtr->GetMaterial();

			m_Registry.AddComponent<MeshRenderer>(entity, MeshRenderer(meshPtr, model, materialPtr));

			// Handle multi-mesh node splits by creating sub-entities for additional meshes
			for (size_t i = 1; i < node.MeshesIndices.size(); ++i)
			{
				uint32_t extraMeshIndex = node.MeshesIndices[i];
				auto extraMeshPtr = model->GetMeshes()[extraMeshIndex];
				auto extraMaterialPtr = extraMeshPtr->GetMaterial();

				uint32_t subMeshEntity = CreateEntity(node.Name + "_Mesh_" + std::to_string(i));
				SetParent(subMeshEntity, entity);

				m_Registry.AddComponent<MeshRenderer>(subMeshEntity, MeshRenderer(extraMeshPtr, model, extraMaterialPtr));
			}
		}

		// Traverse and instantiate all child nodes
		for (const auto& childNode : node.Children)
		{
			InstantiateNode(childNode, model, entity);
		}

		return entity;
	}

	void Scene::UpdateSceneTransforms()
	{
		auto view = m_Registry.GetView<Transform>();

		for (uint32_t entityID : view)
		{
			bool isRoot = true;

			if (m_Registry.HasComponent<RelationshipComponent>(entityID))
			{
				auto& rel = m_Registry.GetComponent<RelationshipComponent>(entityID);
				if (rel.Parent != NullEntity)
					isRoot = false; // Sub-tree updates will handle this entity
			}

			// Start recursion from Root entities using Identity Matrix
			if (isRoot)
			{
				UpdateEntityTransform(entityID, glm::mat4(1.0f));
			}
		}
	}

	bool Scene::IsDescendant(uint32_t possibleAncestorID, uint32_t entityID)
	{
		// 1. Self-parenting is invalid
		if (possibleAncestorID == entityID)
			return true;

		if (!m_Registry.HasComponent<RelationshipComponent>(entityID))
			return false;

		// 2. Walk up the parent hierarchy from 'entityID'
		uint32_t currentParent = m_Registry.GetComponent<RelationshipComponent>(entityID).Parent;

		while (currentParent != NullEntity)
		{
			if (currentParent == possibleAncestorID)
				return true; // Loop detected!

			if (!m_Registry.HasComponent<RelationshipComponent>(currentParent))
				break;

			currentParent = m_Registry.GetComponent<RelationshipComponent>(currentParent).Parent;
		}

		return false; // Safe to parent
	}

	uint32_t Scene::CreateEntity(const std::string& name)
	{
		uint32_t entity = m_Registry.CreateEntity();

		m_Registry.AddComponent<IdentityComponent>(entity, IdentityComponent(name, "Default", 0));
		m_Registry.AddComponent<Transform>(entity, Transform());
		m_Registry.AddComponent<RelationshipComponent>(entity, RelationshipComponent());

		return entity;
	}

	void Scene::CreateDirectionalLightEntity()
	{
		uint32_t directionalLight = CreateEntity("Directional Light");
		m_Registry.AddComponent<LightComponent>(directionalLight, LightComponent());
		auto& transform = m_Registry.GetComponent<Transform>(directionalLight);
		transform.Position = glm::vec3(2.0f, 2.0f, 2.0f);
		transform.Rotation = glm::vec3(45.0f, -45.0f, 0.0f);
	}

	void Scene::SetSkybox(const std::string& path)
	{
		m_Environment.SkyboxPath = path;
		Renderer::SetSkybox(path);
	}

	void Scene::DestroyEntity(uint32_t entity)
	{
		if (m_Registry.HasComponent<RelationshipComponent>(entity))
		{
			auto& rel = m_Registry.GetComponent<RelationshipComponent>(entity);
			// 1. Remove from it's Parent if any
			if (rel.Parent != NullEntity && m_Registry.HasComponent<RelationshipComponent>(rel.Parent))
			{
				auto& parentRL = m_Registry.GetComponent<RelationshipComponent>(rel.Parent);
				std::erase(parentRL.Children, entity);
			}

			// 2. Also Delete it's Children's if any
			//  Make a copy of the vector to avoid iterator invalidation during destruction
			std::vector<uint32_t> childrenCopy = rel.Children;
			for (uint32_t childID : childrenCopy)
			{
				DestroyEntity(childID);
			}
		}
		// 3. Remove entity from registry
		m_Registry.DestroyEntity(entity);
	}
}