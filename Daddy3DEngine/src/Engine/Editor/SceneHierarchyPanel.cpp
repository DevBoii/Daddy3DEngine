#include "SceneHierarchyPanel.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Core/ECS/Registry.h"
#include "Engine/Scene/Components/IdentityComponent.h"
#include "Engine/Scene/Components/RelationshipComponent.h"
#include "Engine/Renderer/Mesh.h"

#include <imgui.h>
#include <string>

namespace DaddyEngine
{
	InspectorPanel SceneHierarchyPanel::s_InspectorPanel;

	void SceneHierarchyPanel::OnImGuiRender()
	{
		Scene* activeScene = SceneManager::GetActiveScene();
		//ImGui::Begin(activeScene->GetName().c_str());
		ImGui::Begin("Scene Hierarchy");

		if (!activeScene)
		{
			ImGui::Text("No Active Scene");
			ImGui::End();
			return;
		}

		if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_MouseButtonRight))
		{
			if (ImGui::MenuItem("Create Empty Entity"))
			{
				CreatEmptyEntity(activeScene);
			}

			ImGui::EndPopup(); // Popup context close
		}

		if (ImGui::CollapsingHeader(activeScene->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			auto& registry = activeScene->GetRegistry();
			auto identityView = registry.GetView<IdentityComponent>();

			std::vector<uint32_t> entitiesToDestroy;

			// Render Only Root Entity (Parent == NullEntity)
			for (uint32_t entityID : identityView)
			{
				bool isRoot = true;
				if (registry.HasComponent<RelationshipComponent>(entityID))
				{
					auto& rl = registry.GetComponent<RelationshipComponent>(entityID);
					if (rl.Parent != NullEntity)
						isRoot = false; // Skip child entities draw under parent
				}

				if (isRoot)
				{
					DrawEntityNode(registry, entityID, entitiesToDestroy, activeScene);
				}
			}

			if (!entitiesToDestroy.empty())
			{
				for (uint32_t entityID : entitiesToDestroy)
					activeScene->DestroyEntity(entityID);
				m_SelectedEntities.clear();
			}
		}

		// -------------------------------------------------------------------------
		// FIX: Stretch a dummy target to fill all remaining window space
		// -------------------------------------------------------------------------
		ImVec2 availableSpace = ImGui::GetContentRegionAvail();
		// Ensure minimum height so there's always a hit box even if full
		if (availableSpace.y < 50.0f) availableSpace.y = 50.0f;

		ImGui::Dummy(availableSpace);

		if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && !ImGui::IsAnyItemHovered())
		{
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_HIERARCHY_ENTITY"))
				{
					uint32_t draggedEntityID = *(const uint32_t*)payload->Data;

					// Passing NullEntity detaches the child and promotes it to a root entity!
					activeScene->SetParent(draggedEntityID, NullEntity);
				}
				// 2. Instantiate Model dropped from Content Browser as Root Entity
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MODEL_PATH"))
				{
					const char* modelPath = (const char*)payload->Data;
					std::filesystem::path path(modelPath);

					// Load model and instantiate node hierarchy
					auto modelAsset = std::make_shared<Model>(modelPath);
					uint32_t instantiatedRoot = activeScene->InstantiateModel(modelAsset, path.stem().string());

					// Select newly created entity tree
					m_SelectedEntities.clear();
					m_SelectedEntities.insert(instantiatedRoot);
				}
				ImGui::EndDragDropTarget();
			}
		}

		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(Registry& registry, uint32_t entityID, std::vector<uint32_t>& entitiesToDestroy, Scene* activeScene)
	{
		// 1. Get Entity Name to display it
		auto& identity = registry.GetComponent<IdentityComponent>(entityID);
		const char* entityName = identity.Name.c_str();
		bool isSelected = (m_SelectedEntities.find(entityID) != m_SelectedEntities.end());

		// 2. Check if entity has children or not
		// if it has children then expand it and shows its children
		bool hasChildren = false;
		if (registry.HasComponent<RelationshipComponent>(entityID))
		{
			auto& relationship = registry.GetComponent<RelationshipComponent>(entityID);
			hasChildren = !relationship.Children.empty();
		}

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

		if (!hasChildren)
			flags |= ImGuiTreeNodeFlags_Leaf;
		if (isSelected)
			flags |= ImGuiTreeNodeFlags_Selected;

		// Isolate this node UI ID so identical names don't conflict
		ImGui::PushID(static_cast<int>(entityID));

		bool opened = ImGui::TreeNodeEx((void*)(uint32_t)entityID, flags, "%s", entityName);

		// DRAG SOURCE : start dragging this entity
		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("SCENE_HIERARCHY_ENTITY", &entityID, sizeof(uint32_t));
			ImGui::Text("%s", entityName);
			ImGui::EndDragDropSource();
		}
		// DRAG TARGET : Drop another entity onto this node to parent it
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_HIERARCHY_ENTITY"))
			{
				uint32_t draggedEntityID = *(const uint32_t*)payload->Data;
				if(activeScene) activeScene->SetParent(draggedEntityID, entityID);
			}
			ImGui::EndDragDropTarget();
		}

		// Handle Entity Selection
		if (ImGui::IsItemClicked())
		{
			if (ImGui::GetIO().KeyCtrl)
			{
				if (isSelected) m_SelectedEntities.erase(entityID);
				else m_SelectedEntities.insert(entityID);
			}
			else
			{
				m_SelectedEntities.clear();
				m_SelectedEntities.insert(entityID);
			}
		}

		// Handle Entity Removal Context Menu
		if (ImGui::BeginPopupContextItem())
		{
			// 1. Safety Guard: Ensure the set is not empty
			if (!m_SelectedEntities.empty())
			{
				if (ImGui::MenuItem("Create Parent Entity"))
				{
					// 2. Safely grab the first element from the set using an iterator
					uint32_t childEntity = *m_SelectedEntities.begin();

					// 3. Create and link the new entity
					uint32_t parentEntity = CreatEmptyEntity(activeScene);
					if (activeScene->GetRegistry().GetComponent<RelationshipComponent>(childEntity).Parent != NullEntity)
					{
						activeScene->SetParent(parentEntity, activeScene->GetRegistry().GetComponent<RelationshipComponent>(childEntity).Parent);
					}
					activeScene->SetParent(childEntity, parentEntity);

					// 4. Optional UX: Selection handling
					 m_SelectedEntities.clear();
					 m_SelectedEntities.insert(childEntity);
				}

				if (ImGui::MenuItem("Create Child Entity"))
				{
					// 2. Safely grab the first element from the set using an iterator
					uint32_t parentEntity = *m_SelectedEntities.begin();

					// 3. Create and link the new entity
					uint32_t childEntity = CreatEmptyEntity(activeScene);
					activeScene->SetParent(childEntity, parentEntity);

					// 4. Optional UX: Selection handling
					 m_SelectedEntities.clear();
					 m_SelectedEntities.insert(childEntity);
				}
			}

			if (ImGui::MenuItem("Remove Entity"))
			{
				// If the right-clicked item isn't in selection, select only it
				if (m_SelectedEntities.find(entityID) == m_SelectedEntities.end())
				{
					m_SelectedEntities.clear();
					m_SelectedEntities.insert(entityID);
				}

				// Queue all selected entities for destruction
				for (uint32_t selectedID : m_SelectedEntities)
				{
					entitiesToDestroy.push_back(selectedID);
				}
			}
			ImGui::EndPopup();
		}

		// Recursively draw children if the Tree Node is expanded
		if (opened)
		{
			if (hasChildren)
			{
				auto& rl = registry.GetComponent<RelationshipComponent>(entityID);
				// FIX: Make a copy to prevent iterator invalidation
				auto childrenCopy = rl.Children;
				for (auto childID : childrenCopy)
				{
					DrawEntityNode(registry, childID, entitiesToDestroy, activeScene);
				}
			}
			ImGui::TreePop();
		}

		ImGui::PopID();
	}

	uint32_t SceneHierarchyPanel::CreatEmptyEntity(Scene* activeScene)
	{
		auto& registry = activeScene->GetRegistry();

		std::string dynamicName = "Entity" + std::to_string(registry.GetEntityCount());
		uint32_t entity = activeScene->CreateEntity(dynamicName);
		return entity;
	}
}