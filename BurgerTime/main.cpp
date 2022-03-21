
#include <tei.h>

#include "Level.h"
#include "Fps.h"
#include "Achievements.h"

void TeiClientInit()
{
	auto& scene = tei::Scenes->AddScene();
	tei::Scenes->SetSceneState(scene, true);

	auto& visual = scene.AddChild();
	visual.AddComponent<tei::components::ObjectTransform>(tei::unit::Scale{ 2 });

	{
		auto& bg = visual.AddChild();

		bg.AddComponent<tei::components::ObjectTransform>();
		bg.AddComponent<tei::components::TextureRenderComponent>();
		auto& bgTex = bg.AddComponent(tei::Resources->LoadShared<tei::resource::Texture>("resources/background.jpg"));

		tei::Application->SetWindowProperty(tei::unit::Scale{ bgTex->size * 2.f });
	}

	{
		auto& game  = visual.AddChild();
		CreateAchievements(game);
		CreateLevel(game);
	}

	{
		CreateFps(scene);
	}

	{
		tei::Input->AddCommand(
			tei::input::KeyboardInput::Main::ESCAPE,
			[] { tei::Application->Quit(); }
		);
	}

	tei::Application->SetWindowProperty(tei::application::WindowProperty::RESIZABLE);

}

void TeiClientCleanup()
{}