#pragma once

#include "SceneHierarchyPanel.h"
#include "InspectorPanel.h"
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
		DaddyEngine::Scene* m_ActiveScene = nullptr;

	public:

		void OnStart()
		{
			DaddyEngine::DebugRenderer::Init();
		}

		void OnImGuiRender()
		{
			// Fetch scene pointer through explicit engine context routing
			m_ActiveScene = DaddyEngine::SceneManager::GetActiveScene();

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