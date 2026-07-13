#include "SceneHierarchyPanel.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Core/ECS/Registry.h"
#include "Engine/Scene/Components/IdentityComponent.h"
#include "Engine/Renderer/Mesh.h"

#include <imgui.h>
#include <string>

namespace DaddyEngine
{
	InspectorPanel SceneHierarchyPanel::s_InspectorPanel;

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		Scene* activeScene = SceneManager::GetActiveScene();

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

			for (uint32_t entityID : identityView)
			{
				auto& identity = registry.GetComponent<IdentityComponent>(entityID);
				bool is_Selected = (m_SelectedEntities.find(entityID) != m_SelectedEntities.end());

				//Prevent ID Collisions(ImGui unique hashing rule)
				ImGui::PushID(static_cast<int>(entityID));

				if (ImGui::Selectable(identity.Name.c_str(), is_Selected))
				{
					if (ImGui::GetIO().KeyCtrl)
					{
						if (is_Selected)
						{
							m_SelectedEntities.erase(entityID);
						}
						else
						{
							m_SelectedEntities.insert(entityID);
						}
					}
					else
					{
						m_SelectedEntities.clear();
						m_SelectedEntities.insert(entityID);
					}
				}
				ImGui::PopID(); // Balance the PushID boundary
			}
		}

		ImGui::End();
	}

	void SceneHierarchyPanel::CreatEmptyEntity(Scene* activeScene)
	{
		auto& registry = activeScene->GetRegistry();

		std::string dynamicName = "Entity" + std::to_string(registry.GetEntityCount());
		activeScene->CreateEntity(dynamicName);
	}
}