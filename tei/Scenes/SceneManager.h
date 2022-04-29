#pragma once

#include <vector>
#include <memory>
#include <ranges>

#include <tei/internal/ecs.h>
#include <tei/utility.h>

namespace tei::internal::scene
{

	using Scene = ecs::Object;

	class SceneManager
	{
	public:

		SceneManager();

		bool IsActive() const noexcept;

		// Add a Scene
		Scene& AddScene(bool active = false);

		// Remove a Scene
		void RemoveScene(Scene const&);

		void SetSceneState(Scene const& scene, bool active);

		// View of all Scenes
		auto GetAllScenes() const;
		
		// View of all active Scenes
		auto GetActiveScenes() const;
		
		// View of all inactive Scenes
		auto GetInactiveScenes() const;

		void Do(ecs::detail::MessageKind auto);

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

	inline bool SceneManager::IsActive() const noexcept
	{
		return m_Object.IsActive();
	}

	inline void SceneManager::Do(ecs::detail::MessageKind auto message)
	{
		m_Object.Do(message);
	}

	inline auto SceneManager::GetAllScenes() const
	{
		return m_Object.GetAllChildren();
	}
	
	inline auto SceneManager::GetActiveScenes() const
	{
		return m_Object.GetActiveChildren();
	}
	
	inline auto SceneManager::GetInactiveScenes() const
	{
		return m_Object.GetInactiveChildren();
	}

}