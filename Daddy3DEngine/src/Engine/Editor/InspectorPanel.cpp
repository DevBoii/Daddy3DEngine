#define _CRT_SECURE_NO_WARNINGS // Disable the warning
#include "InspectorPanel.h"

#include <algorithm>
#include <glm/gtc/type_ptr.hpp>

namespace DaddyEngine
{
	void InspectorPanel::OnImGuiRender(Scene* activeScene, const std::set<uint32_t>& selectedEntities)
	{
		ImGui::Begin("Inspector");
		float windowWidth = ImGui::GetContentRegionAvail().x;
		if (!activeScene || selectedEntities.empty())
		{
			ImGui::SetCursorPosX(windowWidth * 0.35f);
			ImGui::Text("No Entity Selected");
			ImGui::End();
			return;
		}

		auto& registry = activeScene->GetRegistry();
		uint32_t selectedEntity = *selectedEntities.begin();
		
		// Selected Entity On Top Always
		if (registry.HasComponent<IdentityComponent>(selectedEntity))
		{
			auto& identity = registry.GetComponent<IdentityComponent>(selectedEntity);

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			std::strncpy(buffer, identity.Name.c_str(), sizeof(buffer));
			ImGui::Checkbox("##IsActive", &identity.isActive);
			ImGui::SameLine();

			if (ImGui::InputText("##Name", buffer, sizeof(buffer)))
			{
				identity.Name = std::string(buffer);
			}

			ImGui::Text("Tag");
			ImGui::SameLine();
			bool openCreateTagPopup = false;
			if (ImGui::BeginCombo("##Tag", identity.Tag.c_str()))
			{
				for (const auto& tag : DaddyEngine::m_Tags)
				{
					bool selected = (identity.Tag == tag);

					if (ImGui::Selectable(tag.c_str(), selected))
						identity.Tag = tag;

					if (selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::Separator();
				if (ImGui::Selectable("+ Add New Tag"))
				{
					openCreateTagPopup = true;
				}
				
				ImGui::EndCombo();
			}
			if(openCreateTagPopup)
				ImGui::OpenPopup("CreateTag");

			if (ImGui::BeginPopup("CreateTag"))
			{
				ImGui::Text("Create a new tag");

				static char tagBuffer[64];
				ImGui::InputText("Tag Name", tagBuffer, sizeof(tagBuffer));

				if (ImGui::Button("Create"))
				{
					if (strlen(tagBuffer) > 0)
					{
						auto it = std::find(DaddyEngine::m_Tags.begin(), DaddyEngine::m_Tags.end(), std::string(tagBuffer));

						if (it == DaddyEngine::m_Tags.end())
						{
							// Tag doesn't exist
							DaddyEngine::m_Tags.push_back(tagBuffer);
							identity.Tag = tagBuffer;
						}
						ImGui::CloseCurrentPopup();
					}
				}

				ImGui::SameLine();

				if (ImGui::Button("Cancel"))
				{
					tagBuffer[0] = '\0';
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}
		}

		ImGui::Separator();
		ImGui::Spacing();

		std::apply([&](auto... args) 
			{
				(this->DrawComponentNode<decltype(args)>(typeid(decltype(args)).name(), selectedEntity, registry), ...);
			}
		, ComponentList{});

		// Add Component
		float buttonWidth = 150.0f;
		float buttonHeight = 30.0f;
		
		ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);

		if (ImGui::Button("Add Component", ImVec2(buttonWidth, buttonHeight)))
		{
			ImGui::OpenPopup("AddComponentPopup");
		}

		if (ImGui::BeginPopup("AddComponentPopup"))
		{
			std::apply([&](auto... args)
				{
					(
						[&]()
						{
							using ComponentType = decltype(args);

							if (!registry.HasComponent<ComponentType>(selectedEntity))
							{
								std::string name = GetComponentName<ComponentType>();

								if (ImGui::MenuItem(name.c_str()))
								{
									registry.AddComponent<ComponentType>(selectedEntity, ComponentType{});
									ImGui::CloseCurrentPopup();
								}
							}
						}(),
							...
							);
				}, ComponentList{});
			
			ImGui::EndPopup();
		}

		ImGui::End();
	}
}