#pragma once

#include "Engine/Scene/Scene.h"
#include "Engine/Core/Camera.h"

namespace Sandbox
{
	class SandboxScene : public DaddyEngine::Scene
	{
	private:

	public:
		SandboxScene() : DaddyEngine::Scene("Sandbox_Level") {}
		virtual ~SandboxScene() override = default; // Crucial for virtual classes!

		virtual void OnCreate() override;
		virtual void OnUpdate(DaddyEngine::Timestep s, DaddyEngine::Camera& camera) override;
		virtual void OnDestroy() override;

	};
}