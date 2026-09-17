#pragma once

#include "SceneHierarchyPanel.h"
#include "InspectorPanel.h"
#include "EnvironmentPanel.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Core/Camera.h"
#include "Engine/Core/Application.h"
#include "Engine/Renderer/DebugRenderer.h"
#include "GizmoSettings.h"

#include <set>

namespace DaddyEditor
{
	class EditorLayer
	{
	private:
		DaddyEngine::SceneHierarchyPanel m_SceneHierarchyPanel;
		DaddyEngine::InspectorPanel      m_InspectorPanel;
		DaddyEngine::EnvironmentPanel	 m_EnvironmentPanel;
		DaddyEngine::Scene* m_ActiveScene = nullptr;

	public:

		void OnStart()
		{
			DaddyEngine::DebugRenderer::Init();
		}

		void OnImGuiRender()
		{
			if (ImGui::BeginMainMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					
				}

				if (ImGui::BeginMenu("Edit"))
				{
					
				}

				if (ImGui::BeginMenu("Assets"))
				{
					ImGui::MenuItem("Import");
					ImGui::MenuItem("Create");
					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Window"))
				{
					if (ImGui::MenuItem("Environment"))
					{
						m_EnvironmentPanel.isOpen = true;
					}
					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Help"))
				{
					ImGui::MenuItem("About");
					ImGui::EndMenu();
				}

				ImGui::EndMainMenuBar();
			}

			// Fetch scene pointer through explicit engine context routing
			m_ActiveScene = DaddyEngine::SceneManager::GetActiveScene();

			if (m_ActiveScene)
			{
				m_EnvironmentPanel.SetEnvironment(
					m_ActiveScene->GetEnvironment()
				);

				m_EnvironmentPanel.OnImGuiRender(m_ActiveScene);
			}

			// Render Panels
			m_SceneHierarchyPanel.OnImGuiRender();

			// Bridge selection data to the Inspector loop
			const std::set<uint32_t>& selections = m_SceneHierarchyPanel.GetSelectedEntities();
			m_InspectorPanel.OnImGuiRender(m_ActiveScene, selections);

			// Submit Transform Gizmos for all selected entities
			for (uint32_t entityID : selections)
			{
				if (m_ActiveScene->GetRegistry().HasComponent<DaddyEngine::Transform>(entityID))
				{
					auto& transform = m_ActiveScene->GetRegistry().GetComponent<DaddyEngine::Transform>(entityID);
					if(DaddyEngine::GizmoSettings::debugTransform)
						DaddyEngine::DebugRenderer::DrawTransform(transform, 1.0f);
				}
			}
		}
	};
}