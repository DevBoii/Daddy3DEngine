#pragma once
#include "Engine/Editor/EditorLayer.h"

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
        DaddyEditor::EditorLayer s_EdditorPanel;

    private:
        DaddyEngine::Application& m_App;
        void ProfilerWindow(Timestep ts);
        void InspectorWindow();
        void SceneWindow();
        void MainWindow();
    };
}