#include "ImGuiLayer.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include "Engine/Core/Application.h"
#include "Engine/Core/Timestep.h"
#include "Engine/Core/Camera.h"
#include "Engine/Renderer/FrameBuffer.h"

#include <cstring>

namespace DaddyEngine
{
    bool ImGuiLayer::g_SceneWindowHovered = false;
    ImGuiLayer::ImGuiLayer()
        : m_App(Application::Get())
    {

    }

    ImGuiLayer::~ImGuiLayer() {}

    void ImGuiLayer::OnAttach()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        ImGui::StyleColorsDark();

        GLFWwindow* window = static_cast<GLFWwindow*>(m_App.GetWindow());

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");
    }

    void ImGuiLayer::OnDetach()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void ImGuiLayer::Begin(Timestep ts)
    {
        ImGuiIO& io = ImGui::GetIO();
        // Feed ImGui your engine's delta time
        io.DeltaTime = ts.GetSeconds() > 0.0f ? ts.GetSeconds() : 0.00001f;
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void ImGuiLayer::End()
    {
        ImGuiIO& io = ImGui::GetIO();
        
        io.DisplaySize = ImVec2((float)m_App.GetWidth(), (float)m_App.GetHeight());

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void ImGuiLayer::OnImGuiRender(Timestep ts)
    {
        // Use PassthruCentralNode so your game scene is visible in the background
        ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), dockspace_flags);

        //MainWindow();
        SceneWindow();
        ProfilerWindow(ts);
        s_EdditorPanel.OnImGuiRender();
    }

    void ImGuiLayer::ProfilerWindow(Timestep ts)
    {
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;

        if (ImGui::Begin("Output", nullptr, flags))
        {
            // 1. Start the Tab Bar
            if (ImGui::BeginTabBar("BottomTabs"))
            {
                // 2. The Profiler Tab
                if (ImGui::BeginTabItem("Profiler"))
                {
                    float fps = ts.GetSeconds() > 0.0f ? (1.0f / ts.GetSeconds()) : 0.0f;
                    ImGui::Text("FPS: %.1f", fps);
                    ImGui::Text("Frame Time: %.3f ms", ts.GetMilliseconds());
                    ImGui::EndTabItem();
                }

                // 3. The Console Tab
                if (ImGui::BeginTabItem("Console"))
                {
                    ImGui::TextColored(ImVec4(1, 1, 0, 1), "[Warning]: Low light in scene.");
                    if (ImGui::Button("Clear Log")) { /* Logic */ }
                    ImGui::EndTabItem();
                }

                ImGui::EndTabBar();
            }
        }
        ImGui::End();
    }

    void ImGuiLayer::SceneWindow()
    {
        Camera* camera = m_App.GetEditorCamera();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar;

        ImGui::Begin("Editor", nullptr, window_flags);

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("CameraSettings", window_flags))
            {
                if (camera)
                {
                    ImGui::SliderFloat("Speed", &camera->cameraSpeed, 0.5f, 50.0f);
                    if (ImGui::Button("Reset Speed")) camera->cameraSpeed = 5.0f;

                    ImGui::Separator();

                    if (ImGui::SliderFloat("Fov", &camera->m_FOV, 0.5f, 120.0f))
                        camera->CalculateVP();
                    if (ImGui::Button("Reset FOV"))
                    {
                        camera->m_FOV = 45.0f;
                        camera->CalculateVP();
                    }
                }
                ImGui::EndMenu();
            }

            ImGui::Checkbox("WireFrameMode", &m_App.m_WireframeMode);

            ImGui::EndMenuBar();
        }

        g_SceneWindowHovered = ImGui::IsWindowHovered();
        // Get the size of specific ImGui window
        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

        Framebuffer* fb = m_App.GetFramebuffer();

        //Resize FB and Camera if the window size changed
        if (fb->width != viewportPanelSize.x || fb->height != viewportPanelSize.y)
        {
            fb->Resize((int)viewportPanelSize.x, (int)viewportPanelSize.y);
            camera->SetViewportSize(viewportPanelSize.x, viewportPanelSize.y);
        }

        //Draw the texture (flipped because OpenGL is bottom-up)
        uint32_t textureID = fb->m_texture;
        ImGui::Image((void*)(uintptr_t)textureID, viewportPanelSize, ImVec2(0, 1), ImVec2(1, 0));

        ImGui::End();
        ImGui::PopStyleVar();
    }

    void ImGuiLayer::MainWindow()
    {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
    }
}