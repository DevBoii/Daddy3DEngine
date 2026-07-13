#pragma once

#include "SceneHierarchyPanel.h"
#include "InspectorPanel.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/SceneManager.h"
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
		void OnImGuiRender()
		{
			// Fetch scene pointer through explicit engine context routing
			m_ActiveScene = DaddyEngine::SceneManager::GetActiveScene();

			// Render Panels
			m_SceneHierarchyPanel.OnImGuiRender();

			// Bridge selection data to the Inspector loop
			const std::set<uint32_t>& selections = m_SceneHierarchyPanel.GetSelectedEntities();
			m_InspectorPanel.OnImGuiRender(m_ActiveScene, selections);
		}
	};
}