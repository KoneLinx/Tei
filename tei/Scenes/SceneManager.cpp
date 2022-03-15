
#include "SceneManager.h"

#include <ranges>
#include <algorithm>

#include <tei/internal/utility.h>

namespace tei::internal::scene
{
	utility::Service<SceneManager> Scenes{};

	SceneManager::SceneManager()
		: m_Object{ nullptr, true }
	{}
	
	Scene& tei::internal::scene::SceneManager::AddScene(bool active)
	{
		auto& obj{ m_Object.AddChild(active) };
		obj.m_pParent = nullptr;
		return obj;
	}

	void SceneManager::RemoveScene(Scene const& scene)
	{
		m_Object.RemoveChild(scene);
	}

	void SceneManager::SetSceneState(Scene const& scene, bool active)
	{
		auto children{ m_Object.GetAllChildren() };
		auto it{ std::ranges::find(children, &scene, utility::addressof_projector{}) };
		if (it == std::ranges::end(children))
			throw utility::TeiRuntimeError{ "This scene does not exist" };
		(*it).SetState(active);
	}

}
