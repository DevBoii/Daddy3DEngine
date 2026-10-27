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
#include "Engine/Editor/GizmoSettings.h"

#include <cstring>

namespace DaddyEngine
{
    bool ImGuiLayer::g_SceneWindowHovered = false;

    glm::vec2 ImGuiLayer::g_SceneViewportPosition(0.0f);
    glm::vec2 ImGuiLayer::g_SceneViewportSize(0.0f);
    glm::vec2 ImGuiLayer::g_SceneMousePosition(0.0f);

    bool ImGuiLayer::g_MouseInsideScene = false;

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
        s_EditorPanel.OnStart();
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

        s_EditorPanel.OnImGuiRender();
        SceneWindow();
        contentBrowserPanel.OnImGuiRender();
        consolePanel.OnImGuiRender();
        ProfilerPanel::OnImGuiRender(ts);
    }

    void ImGuiLayer::SceneWindow()
    {
        Camera* camera = m_App.GetEditorCamera();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar;

        ImGui::Begin("Editor", nullptr, window_flags);

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("CameraSettings"))
            {
                if (camera)
                {
                    ImGui::SliderFloat("Speed", &camera->cameraSpeed, 0.5f, 50.0f);
                    if (ImGui::Button("Reset Speed")) camera->cameraSpeed = 5.0f;
                    ImGui::Separator();

                    ImGui::SliderFloat("Camera sensitivity", &camera->sensitivity, 0.1f, 1.0f);
                    if (ImGui::Button("Reset sensitivity")) camera->sensitivity = 0.1f;
                    ImGui::Separator();

                    if (ImGui::SliderFloat("Camera Near Plane", &camera->m_nearPlane, 0.1f, 10.0f))
                        camera->CalculateVP();
                    if (ImGui::Button("Reset Camera Near Plane"))
                    {
                        camera->m_nearPlane = 0.1f;
                        camera->CalculateVP();
                    }
                    if (ImGui::SliderFloat("Camera Far Plane", &camera->m_farPlane, 0.1f, 500.0f))
                        camera->CalculateVP();
                    if (ImGui::Button("Reset Camera Far Plane"))
                    {
                        camera->m_farPlane = 100.0f;
                        camera->CalculateVP();
                    }

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

            ImGui::Checkbox("WireFrame", &m_App.m_WireframeMode);

            ImGui::Checkbox("##drawGizmos", &DaddyEngine::GizmoSettings::drawGizmos);
            if (ImGui::BeginMenu("Gizmos"))
            {
                ImGui::Checkbox("Transform", &DaddyEngine::GizmoSettings::debugTransform);
                ImGui::Checkbox("DirectionalLight", &DaddyEngine::GizmoSettings::debugDirectionalLight);
                ImGui::Checkbox("Grid", &DaddyEngine::GizmoSettings::debugGrid);
                ImGui::Checkbox("WorldAxis", &DaddyEngine::GizmoSettings::debugWorldAxis);

                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        g_SceneWindowHovered = ImGui::IsWindowHovered();
        // Get the size of specific ImGui window
        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        ImVec2 imageMin = ImGui::GetCursorScreenPos();

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

        // Read mouse position in screen coordinates
        ImVec2 mouse = ImGui::GetMousePos();

        // Store viewport information for terrain editing
        g_SceneViewportPosition = glm::vec2(imageMin.x, imageMin.y);
        g_SceneViewportSize = glm::vec2(viewportPanelSize.x, viewportPanelSize.y);

        // Convert screen coordinates to image-relative coordinates
        g_SceneMousePosition = glm::vec2(mouse.x - imageMin.x, mouse.y - imageMin.y);

        // Check whether the mouse is inside the image
        g_MouseInsideScene = mouse.x >= imageMin.x && mouse.y >= imageMin.y && mouse.x < imageMin.x + viewportPanelSize.x && mouse.y < imageMin.y + viewportPanelSize.y;

        ImGui::End();
        ImGui::PopStyleVar();
    }
}