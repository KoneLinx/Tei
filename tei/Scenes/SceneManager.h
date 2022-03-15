#pragma once

#include <vector>
#include <memory>
#include <ranges>

#include <tei/internal/ecs.h>
#include <tei/internal/utility.h>

namespace tei::internal::scene
{

	using Scene = ecs::Object;

	class SceneManager
	{
	public:

		SceneManager();

		bool IsActive() const;

		// Add a Scene
		Scene& AddScene(bool active = false);

		// Remove a Scene
		void RemoveScene(Scene const&);

		void SetSceneState(Scene const& scene, bool active);

		// View of all Scenes
		std::ranges::view auto GetAllScenes() const;
		
		// View of all active Scenes
		std::ranges::view auto GetActiveScenes() const;
		
		// View of all inactive Scenes
		std::ranges::view auto GetInactiveScenes() const;

		void Do(ecs::Message);

	private:

		Scene m_Object;

	};

	extern utility::Service<SceneManager> Scenes;
}

namespace tei::external
{
	inline constexpr auto const& Scenes{ tei::internal::scene::Scenes };
}

namespace tei::internal::scene
{

	inline bool SceneManager::IsActive() const
	{
		return false;
	}

	inline void SceneManager::Do(ecs::Message message)
	{
		m_Object.Do(message);
	}


	inline std::ranges::view auto SceneManager::GetAllScenes() const
	{
		return m_Object.GetAllChildren();
	}

	inline std::ranges::view auto SceneManager::GetActiveScenes() const
	{
		return m_Object.GetActiveChildren();
	}

	inline std::ranges::view auto SceneManager::GetInactiveScenes() const
	{
		return m_Object.GetInactiveChildren();
	}

}