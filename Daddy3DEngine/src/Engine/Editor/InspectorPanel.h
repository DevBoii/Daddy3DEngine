#pragma once

#include "Engine/Scene/Scene.h"
#include <cstdint>
#include <set>
#include <imgui.h>

#include "Engine/Scene/Components/IdentityComponent.h"
#include "Engine/Scene/Components/MeshRenderer.h"
#include "Engine/Renderer/Mesh.h"
#include "Engine/Renderer/Transform.h"
#include "Engine/Core/Assets/AssetManager.h"

namespace DaddyEngine
{
	class InspectorPanel
	{
	private:
		template<typename T>
		std::string GetComponentName()
		{
			std::string name = typeid(T).name();
			if (name.find("struct ") == 0) name.erase(0, 7);
			if (name.find("class ") == 0) name.erase(0, 6);
			if (name.find("DaddyEngine::") == 0) name.erase(0, 13);
			return name;
		}

		template<typename T>
		void DrawComponentNode(const std::string& name, uint32_t entity, Registry& registry)
		{
			//Check if entity has that component then only draw tree
			if (registry.HasComponent<T>(entity))
			{
				auto& component = registry.GetComponent<T>(entity);

				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth;

				std::string cleanName = GetComponentName<T>();

				// BeginPopupContextItem() opens a context menu for the previous ImGui item
				bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), flags, cleanName.c_str());

				bool removeComponent = false;
				if (ImGui::BeginPopupContextItem())
				{
					if (!std::is_same_v<T, Transform>)
					{
						if (ImGui::MenuItem("Remove Component"))
							removeComponent = true;
					}
					ImGui::EndPopup();
				}

				if (open)
				{
					// Here you can specify how each component should be drawn.
					if constexpr (std::is_same_v < T, Transform>)
					{
						TransformPanel(component);
					}
					else if constexpr (std::is_same_v<T, MeshRenderer>)
					{
						MeshRendererPanel(component);
					}
					ImGui::TreePop();
				}

				if (removeComponent)
				{
					registry.RemoveComponent<T>(entity);
				}

				ImGui::Spacing();
			}
		}

		void TransformPanel(Transform& component)
		{
			ImGui::Text("Position");
			ImGui::SameLine(100); // X position of the widget
			ImGui::DragFloat3("##Position", glm::value_ptr(component.Position), 0.1f);
			ImGui::Text("Rotation");
			ImGui::SameLine(100);
			ImGui::DragFloat3("##Rotation", glm::value_ptr(component.Rotation), 0.1f);
			ImGui::Text("Scale");
			ImGui::SameLine(100);
			ImGui::DragFloat3("##Scale", glm::value_ptr(component.Scale), 0.1f);
		}

		void MeshRendererPanel(MeshRenderer& component)
		{
			const char* availablePrimitives[] = { "Primitive::Cube", "Primitive::Pyramid", "Primitive::Plane" };

			ImGui::Text("MeshRenderer Component Attached");
			ImGui::Checkbox("Visible", &component.IsVisible);
			ImGui::Checkbox("CastShadow", &component.castShadow);
			ImGui::Spacing();

			ImGui::Text("Current Path: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), component.ModelPath.c_str());
			ImGui::Spacing();

			ImGui::Text("Mesh Asset: ");
			ImGui::SameLine(100);

			if (ImGui::BeginCombo("##MeshSelector", "Select Mesh"))
			{
				for (int i = 0; i < 3; i++)
				{
					bool isSelected = component.ModelPath == availablePrimitives[i];

					if (ImGui::Selectable(availablePrimitives[i], isSelected))
					{
						component.ModelPath = availablePrimitives[i];
						component.MeshAsset = AssetManager::GetOrCreateMesh(component.ModelPath);
					}
					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			// Display Index Count for performance debugging
			if (component.MeshAsset != nullptr)
			{
				ImGui::Spacing();
				ImGui::TextDisabled("Indices Count: %u", component.MeshAsset->GetIndexCount());
			}
		}

		using ComponentList = std::tuple<Transform, MeshRenderer>;

	public:
		InspectorPanel() = default;
		~InspectorPanel() = default;

		void OnImGuiRender(Scene* activeScene, const std::set<uint32_t>& selectedEntities);
	};
}