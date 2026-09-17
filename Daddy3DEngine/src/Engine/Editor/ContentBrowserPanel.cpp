#include "ContentBrowserPanel.h"

#include <imgui.h>

namespace DaddyEngine
{
	ContentBrowserPanel::ContentBrowserPanel()
		:m_BaseDirectory("Assets"), m_CurrentDirectory("Assets")
	{
		if (!std::filesystem::exists(m_BaseDirectory))
		{
			std::filesystem::create_directories(m_BaseDirectory);
		}
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser");

		// Top Navigation Bar
		if (m_CurrentDirectory != m_BaseDirectory)
		{
			if (ImGui::Button("<--"))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
			ImGui::SameLine();
		}
		ImGui::Text("Current Path: %s", m_CurrentDirectory.string().c_str());
		ImGui::Separator();

		// left Panel
		ImGui::BeginChild("Folders", ImVec2(200, 0), true);

		if (std::filesystem::exists(m_BaseDirectory))
		{
			DrawDirectoryTree(m_BaseDirectory);
		}

		ImGui::EndChild();

		ImGui::SameLine();
		// Right Panel ( Content)
		ImGui::BeginChild("Content", ImVec2(0, 0), true);

		// Grid parameters
		float padding = 16.0f;
		float thumbnailSize = 64.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = static_cast<int>(panelWidth / cellSize);
		if (columnCount < 1) columnCount = 1;

		ImGui::Columns(columnCount, nullptr, false);

		if (std::filesystem::exists(m_CurrentDirectory) && !std::filesystem::is_empty(m_CurrentDirectory))
		{
			for (const auto& entry : std::filesystem::directory_iterator(m_CurrentDirectory))
			{
				const auto& path = entry.path();
				std::string fileName = path.filename().string();

				ImGui::PushID(fileName.c_str());
				// Folder
				if (entry.is_directory())
				{
					if (ImGui::Button(("FOLDER"), ImVec2(thumbnailSize, thumbnailSize)))
					{
						m_CurrentDirectory = path;
					}
				}
				else  // File
				{
					std::string extension = path.extension().string();
					ImGui::Button(("FILE" + extension).c_str(), ImVec2(thumbnailSize, thumbnailSize));

					// For Models
					if (extension == ".fbx" || extension == ".obj")
					{
						if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
						{
							std::string relativePath = path.string();
							ImGui::SetDragDropPayload("MODEL_PATH", relativePath.c_str(), relativePath.size() + 1);
							ImGui::Text("Dragging: %s", fileName.c_str());
							ImGui::EndDragDropSource();
						}
					}
					
					//For Textures
					//Check if the file is a texture extension (.png, .jpg, .tga)
					if (extension == ".png" || extension == ".jpg" || extension == ".tga")
					{
						if (ImGui::BeginDragDropSource())
						{
							std::string relativePath = path.string();
							ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM_TEXTURE", relativePath.c_str(), (relativePath.size() + 1) * sizeof(char));
							ImGui::Text("Assign Texture: %s", fileName.c_str());
							ImGui::EndDragDropSource();
						}
					}

					//For Skyboxes
					if (extension == ".skybox")
					{
						if (ImGui::BeginDragDropSource())
						{
							std::string relativePath = path.string();
							ImGui::SetDragDropPayload("SKYBOX_PATH", relativePath.c_str(), (relativePath.size() + 1) * sizeof(char));
							ImGui::Text("Assign Skybox: %s", fileName.c_str());
							ImGui::EndDragDropSource();
						}
					}
				}
				ImGui::TextWrapped("%s", fileName.c_str());
				ImGui::NextColumn();
				ImGui::PopID();
			}
		}
		else
		{
			ImGui::TextDisabled("Folder is empty...");
		}

		ImGui::Columns(1);
		ImGui::EndChild();
		ImGui::End();
	}

	void ContentBrowserPanel::DrawDirectoryTree(const std::filesystem::path& directory)
	{
		for (const auto& entry : std::filesystem::directory_iterator(directory))
		{
			if (!entry.is_directory()) continue;

			bool haschildren = false;

			for (const auto& child : std::filesystem::directory_iterator(entry.path()))
			{
				if (child.is_directory())
				{
					haschildren = true;
					break;
				}
			}

			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
			if(!haschildren)
				flags |= ImGuiTreeNodeFlags_Leaf;
			if (m_CurrentDirectory == entry.path())
				flags |= ImGuiTreeNodeFlags_Selected;

			std::string name = entry.path().filename().string();

			bool opened = ImGui::TreeNodeEx(entry.path().string().c_str(), flags, "%s", name.c_str());

			if (ImGui::IsItemClicked())
			{
				m_CurrentDirectory = entry.path();
			}

			if (opened)
			{
				DrawDirectoryTree(entry.path());
				ImGui::TreePop();
			}
		}
	}
}