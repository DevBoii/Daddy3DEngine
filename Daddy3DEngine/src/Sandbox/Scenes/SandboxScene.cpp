#include "SandboxScene.h"
#include "Engine/Renderer/Transform.h"
#include "Engine/Renderer/Mesh.h"
#include "Engine/Scene/Components/MeshRenderer.h"
#include "Engine/Core/Assets/AssetManager.h"

#include <iostream>

namespace Sandbox
{
	void SandboxScene::OnCreate()
    {
        
    }

	void SandboxScene::OnUpdate(DaddyEngine::Timestep s)
	{
		DaddyEngine::Scene::OnUpdate(s);	// Important
	}

	void SandboxScene::OnDestroy()
	{
		std::cout << "[SandboxScene] Cleaning up level data...\n";
		// Base registry cleanup happens automatically when Scene deletes!
	}
}