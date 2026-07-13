#pragma once

#include "Scene.h"

namespace DaddyEngine
{
	class SceneManager
	{
	private:
		static inline Scene* m_ActiveScene = nullptr;
	public:

		static void SwitchScene(Scene* newScene);

		static Scene* GetActiveScene();

		// Safe cleanup on engine shutdown
		static void Shutdown();
	};
}