#include "EnvironmentPanel.h"
#include <imgui.h>

namespace DaddyEngine
{
    void EnvironmentPanel::OnImGuiRender(Scene* scene)
    {
        if (!isOpen || !m_Environment) return;

        ImGui::Begin("Environment");

        // Right click on the window
        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::MenuItem("Remove Panel"))
            {
                isOpen = false;
            }

            ImGui::EndPopup();
        }

        const char* skyTypes[] =
        {
            "Solid Color",
            "Skybox",
        };

        ImVec2 windowSize = ImGui::GetWindowSize();

        const char* skyboxText = "-Skybox-";
        float skyboxTextWidth = ImGui::CalcTextSize(skyboxText).x;
        ImGui::SetCursorPosX((windowSize.x - skyboxTextWidth) / 2.0f);
        ImGui::TextDisabled("%s", skyboxText);
        ImGui::Separator();

        int current = static_cast<int>(m_Environment->SkyType);
        ImGui::Text("SkyType");
        ImGui::SameLine();
        if (ImGui::Combo("##SkyType", &current, skyTypes, IM_ARRAYSIZE(skyTypes)))
        {
            m_Environment->SkyType = static_cast<DaddyEngine::SkyType>(current);
        }

        switch (m_Environment->SkyType)
        {
        case DaddyEngine::SkyType::SolidColor:
            SolidColorPanel();
            break;
        case DaddyEngine::SkyType::Skybox:
            SkyboxPanel(scene);
            break;
        default:
            break;
        }

        ImGui::Separator();
        const char* fogText = "-Fog-";
        float fogTextWidth = ImGui::CalcTextSize(fogText).x;
        ImGui::SetCursorPosX((windowSize.x - fogTextWidth) / 2.0f);
        ImGui::TextDisabled("%s", fogText);
        ImGui::Separator();

        FogPanel();

        ImGui::End();
    }
    void EnvironmentPanel::SolidColorPanel()
    {
        ImGui::Text("Sky Color: ");
        ImGui::SameLine();
        ImGui::ColorEdit3("##Sky Color", &m_Environment->SkyColor.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_Float);
    }

    void EnvironmentPanel::SkyboxPanel(Scene* scene)
    {
        ImGui::Text("Skybox: ");
        ImGui::SameLine();
        std::string skyboxPath = m_Environment->SkyboxPath;

        std::string fileName = std::filesystem::path(skyboxPath).filename().string();
        if (m_Environment)
        {
            if (skyboxPath.empty())
            {
                ImGui::Button("Drop Skybox Here", ImVec2(250.0f, 30.0f));
            }
            else
            {
                ImGui::Button(fileName.c_str(), ImVec2(250.0f, 30.0f));
            }
        }

        // Drag & Drop target
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SKYBOX_PATH"))
            {
                const char* path = static_cast<const char*>(payload->Data);
                scene->SetSkybox(path);
            }
            ImGui::EndDragDropTarget();
        }
    }

    void EnvironmentPanel::FogPanel()
    {
        ImGui::Text("UseFog: ");
        ImGui::SameLine();
        ImGui::Checkbox("##UseFog: ", &m_Environment->m_FogSettings.useFog);
        ImGui::Text("Fog Color: ");
        ImGui::SameLine();
        ImGui::ColorEdit3("##Fog Color", &m_Environment->m_FogSettings.fogColor.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_Float);
        ImGui::Text("Fog Density: ");
        ImGui::SameLine();
        ImGui::SliderFloat("##Fog Density", &m_Environment->m_FogSettings.fogDensity, 0.0f, 1.0f);
        ImGui::Separator();
        ImGui::Separator();
        ImGui::Text("UseSkyFog: ");
        ImGui::SameLine();
        ImGui::Checkbox("##UseSkyFog: ", &m_Environment->m_FogSettings.useSkyFog);
        ImGui::Text("Fog Sky Color: ");
        ImGui::SameLine();
        ImGui::ColorEdit3("##Fog Sky Color", &m_Environment->m_FogSettings.skyFogColor.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_Float);
        ImGui::SameLine();
        ImGui::Checkbox("##UseSameFogColor: ", &m_Environment->m_FogSettings.useSameColor);
        ImGui::Text("Sky Fog Intensity: ");
        ImGui::SameLine();
        ImGui::SliderFloat("##Sky Fog Intensity", &m_Environment->m_FogSettings.skyFogIntensity, 0.0f, 1.0f);
    }
}