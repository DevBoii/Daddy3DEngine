#pragma once

#include "Engine/Scene/Scene.h"

namespace DaddyEngine
{
    class EnvironmentPanel
    {
    public:
        EnvironmentPanel() = default;

        void SetEnvironment(Environment& environment)
        {
            m_Environment = &environment;
        }

        void OnImGuiRender(Scene* scene);

        bool isOpen = false;

    private:
        void SolidColorPanel();
        void SkyboxPanel(Scene* scene);

        Environment* m_Environment = nullptr;
    };
}