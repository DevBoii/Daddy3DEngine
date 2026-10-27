#pragma once

#include "Engine/Scene/Scene.h"
#include <cstdint>
#include <set>
#include <imgui.h>

#include "Engine/Scene/Components/IdentityComponent.h"
#include "Engine/Scene/Components/MeshRenderer.h"
#include "Engine/Scene/Components/LightComponent.h"
#include "Engine/Scene/Terrain.h"
#include "Engine/Renderer/Mesh.h"
#include "Engine/Renderer/Transform.h"
#include "Engine/Core/Assets/AssetManager.h"
#include "Engine/Renderer/DebugRenderer.h"
#include "Engine/Core/Input.h"

#include <limits>
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
					else if constexpr (std::is_same_v <T, LightComponent>)
					{
						LightComponentPanel(component);
					}
					else if constexpr (std::is_same_v <T, Terrain>)
					{
						TerrainPanel(component);
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
			ImGui::Text("Mesh Renderer");
			ImGui::Checkbox("Visible", &component.IsVisible);
			ImGui::Checkbox("Cast Shadow", &component.CastShadow);

			ImGui::Spacing();

			ImGui::Text("Model");
			ImGui::SameLine(100);

			ImGui::PushID("MeshRenderer");
			const char* buttonText = component.ModelAsset ? std::filesystem::path(component.ModelPath).filename().string().c_str() : "Empty";

			ImGui::Button(buttonText, ImVec2(220, 0));

			// Drag & Drop Target
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MODEL_PATH"))
				{
					const char* path = (const char*)payload->Data;

					component.ModelPath = path;
					component.ModelAsset = AssetManager::GetOrCreateModel(path);
				}

				ImGui::EndDragDropTarget();
			}

			ImGui::TextDisabled("Path: %s", component.ModelPath.c_str());
			ImGui::PopID();

			// --- MATERIAL SECTION ---
			if (ImGui::TreeNodeEx("Material", ImGuiTreeNodeFlags_DefaultOpen))
			{
				// Ensure component has a valid material allocated
				if (!component.MaterialAsset)
					component.MaterialAsset = std::make_shared<Material>();

				// Detect if multiple entities use this same material
				if (component.MaterialAsset.use_count() > 1)
				{
					ImGui::Text("Shared Material (%d users)", component.MaterialAsset.use_count());
					ImGui::SameLine();
				}

				auto& mat = component.MaterialAsset;

				// 1. Color Edit Control
				ImGui::Text("Albedo Color");
				ImGui::SameLine();
				ImGui::ColorEdit3("##Albedo Color", &mat->AlbedoColor.r, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_Float);
				ImGui::Spacing();

				// Albedo Map
				std::string albedoButtonText = component.MaterialAsset->AlbedoMap ? "Texture Loaded" : "None (Drag Texture Here)";
				ImGui::Text("Albedo Map");
				ImGui::SameLine();
				ImGui::Button(albedoButtonText.c_str(), ImVec2(150, 30));

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM_TEXTURE"))
					{
						const char* path = (const char*)payload->Data;
						component.MaterialAsset->AlbedoMap = std::make_shared<Texture2D>(path);
					}
					ImGui::EndDragDropTarget();
				}

				if (component.MaterialAsset->AlbedoMap)
				{
					ImGui::SameLine();
					if (ImGui::Button("X"))
					{
						component.MaterialAsset->AlbedoMap = nullptr;
					}
				}
				ImGui::TreePop();
			}
		}

		void LightComponentPanel(LightComponent& component)
		{
			ImGui::Checkbox("Is Active", &component.isActive);
			ImGui::Spacing();

			ImGui::TextDisabled("Light Settings");

			//Light Type-----------------
			static const char* lightTypeNames[] =
			{
				"Directional",
				"Point",
				"Spot"
			};
			ImGui::Text("Light Type");
			ImGui::SameLine();
			int current = static_cast<int>(component.Type);
			if (ImGui::Combo("##LightType", &current, lightTypeNames, IM_ARRAYSIZE(lightTypeNames)))
			{
				component.Type = static_cast<LightComponent::LightType>(current);
			}
			ImGui::Spacing();
			//-----------------------
			//Light Color
			ImGui::Text("Light Color");
			ImGui::SameLine();
			ImGui::ColorEdit3( "##LightColor", &component.lightColor.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_Float);
			ImGui::Spacing();
			//


			ImGui::Text("Intensity");
			ImGui::SameLine();
			ImGui::SliderFloat("##Intensity", &component.intensity, 0.0f, 10.0f, "%.2f");
			ImGui::Spacing();

			ImGui::Text("Ambient Strength");
			ImGui::SameLine();
			ImGui::SliderFloat("##Ambient Strength", &component.ambientStrength, 0.0f, 1.0f);

			switch (component.Type)
			{
			case LightComponent::LightType::Spot:
			{
				ImGui::Separator();
				ImGui::TextDisabled("Spot Light Settings");
				break;
			}

			case LightComponent::LightType::Point:
			{
				ImGui::Separator();
				ImGui::TextDisabled("Point Light Settings");
				break;
			}
			default:
				break;
			}
		}

		void TerrainPanel(Terrain& component)
		{
			uint16_t minValue = 2;
			uint16_t maxValue = 1000;
			ImGui::Text("Tools");

			ImGui::Text("Terrain Brush");
			ImGui::Separator();

			const char* modes[] =
			{
				"None",
				"Smooth Raise",
				"Hard Raise",
				"Smooth Lower",
				"Hard Lower"
			};

			int currentMode = static_cast<int>(component.brush.mode);

			if (ImGui::Combo("Mode", &currentMode, modes, IM_ARRAYSIZE(modes)))
			{
				component.brush.mode = static_cast<Brush::Mode>(currentMode);
			}
			ImGui::Checkbox("Normals", &component.drawNormals);
			if (component.drawNormals)
			{
				component.DrawNormals();
			}
			if (currentMode != 0)
			{
				ImGui::DragFloat("Radius", &component.brush.radius, 0.1f, 0.1f, 100.0f);

				ImGui::DragFloat("Intensity", &component.brush.intensity, 0.01f, 0.001f, 10.0f);

				ImGui::Text("For Debug");
				
				DaddyEngine::DebugRenderer::DrawLine(glm::vec3(component.modelMatrix * glm::vec4(component.brush.position, 1.0f)), glm::vec3(component.modelMatrix * glm::vec4(component.brush.position, 1.0f)) + glm::vec3(0, -1, 0), glm::vec3(0.1f, 0.4f, 0.7f));
				
				ImGui::DragFloat3("Brush Position", &component.brush.position.x, 0.1f);
				component.EditTerrain();
			}

			ImGui::Separator();

			ImGui::Text("Width: ");
			ImGui::SameLine();
			if (ImGui::DragInt("##Width", &component.width, 1.0f, minValue, maxValue))
			{
				component.GenerateTerrain();
			}
			ImGui::Text("Height: ");
			ImGui::SameLine();
			if(ImGui::DragInt("##Height", &component.height, 1.0f, minValue, maxValue))
			{
				component.GenerateTerrain();
			}
			ImGui::Text("Spacing: ");
			ImGui::SameLine();
			if(ImGui::DragFloat("##Spacing", &component.spacing, 0.1f, 0.1f, 10.0f))
			{
				component.GenerateTerrain();
			}
		}

		using ComponentList = std::tuple<Transform, MeshRenderer, LightComponent, Terrain>;

	public:
		InspectorPanel() = default;
		~InspectorPanel() = default;

		void OnImGuiRender(Scene* activeScene, const std::set<uint32_t>& selectedEntities);
	};
}