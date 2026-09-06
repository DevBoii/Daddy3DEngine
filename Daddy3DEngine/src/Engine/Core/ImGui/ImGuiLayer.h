#pragma once
#include "Engine/Editor/EditorLayer.h"
#include "Engine/Editor/ContentBrowserPanel.h"
#include "Engine/Editor/ConsolePanel.h"
#include "Engine/Editor/ProfilerPanel.h"

namespace DaddyEngine
{
    class Timestep;
    class Application;

    class ImGuiLayer
    {
    public:
        ImGuiLayer();
        ~ImGuiLayer();

        void OnAttach(); // Initialize ImGui
        void OnDetach(); // Shutdown ImGui

        void Begin(Timestep ts);    // Start of the frame
        void End();      // End of the frame (Rendering)

        void OnImGuiRender(Timestep ts); // Where your actual windows live
        static bool g_SceneWindowHovered;
        DaddyEditor::EditorLayer s_EditorPanel;
        ContentBrowserPanel contentBrowserPanel;
        ConsolePanel consolePanel;

    private:
        DaddyEngine::Application& m_App;
        void SceneWindow();
        void MainWindow();
    };
}