#include "SceneManager.h"

namespace DaddyEngine
{
	static inline Scene* m_ActiveScene = nullptr;

	void SceneManager::SwitchScene(Scene* newScene)
	{
		//Wipe out current Active scene
		if (m_ActiveScene)
		{
			m_ActiveScene->OnDestroy();
			delete m_ActiveScene;
			m_ActiveScene = nullptr;
		}

		// assign new Scene to Active Scene
		m_ActiveScene = newScene;

		if (m_ActiveScene)
		{
			m_ActiveScene->OnCreate();
		}
	}

	Scene* SceneManager::GetActiveScene()
	{
		return m_ActiveScene;
	}

	void SceneManager::Shutdown()
	{
		if (m_ActiveScene)
		{
			m_ActiveScene->OnDestroy();
			delete m_ActiveScene;
			m_ActiveScene = nullptr;
		}
	}
}