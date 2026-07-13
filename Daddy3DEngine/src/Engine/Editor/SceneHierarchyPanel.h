#pragma once

#include "Engine/Scene/Scene.h"
#include <set>		// Organizes unique, sorted lists
#include <cstdint>	// Guarantees exact-width integer sizes.

#include "InspectorPanel.h"

namespace DaddyEngine
{
	class SceneHierarchyPanel
	{
	private:
		std::set<uint32_t> m_SelectedEntities;
		static InspectorPanel s_InspectorPanel;

	public:
		SceneHierarchyPanel() = default;
		~SceneHierarchyPanel() = default;

		void OnImGuiRender();

		const std::set<uint32_t>& GetSelectedEntities() const
		{
			return m_SelectedEntities;
		}

		void ClearSelection()
		{
			m_SelectedEntities.clear();
		}

		void CreatEmptyEntity(Scene* activeScene);
	};
}