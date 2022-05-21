#include "pch.h"

#include "FPS.h"

#include "../Loader/LevelLoader.h"

void TeiClientInit()
{

	auto& scene = tei::Scenes->AddScene(true);

	tei::Application->SetWindowTitle("BurgerTime!");
	tei::Application->SetWindowProperty(tei::application::WindowProperty::RESIZABLE);

	// Viewport
	{
		auto& viewport = scene.AddChild();
		auto& viewTransform = viewport.AddComponent<ObjectTransform>();

		// Background
		{
			auto& bgObject = viewport.AddChild();
			auto& bgTransform = bgObject.AddComponent<ObjectTransform>();
			auto& bgTexture = bgObject.AddComponent(tei::Resources->LoadUnique<resource::Texture>("resources/Background.jpg"));
			bgObject.AddComponent<TextureRenderComponent>();

			viewport.AddComponent(
				Event->AddObserver(
					[&viewTransform, &bgTransform, bgRatio = bgTexture->size.x / bgTexture->size.y] (application::WindowPropertyChangedEvent)
					{
						auto& scale = Application->GetWindow().transform.scale;
						viewTransform.get().scale = unit::Scale{ std::min(scale.x, scale.y) };
						bgTransform.get().scale = unit::Scale{ std::max(scale.x / scale.y, scale.y / scale.x) * bgRatio };
					}
				)
			);

			tei::Application->SetWindowProperty(unit::Scale{ 1152, 720 });
		}

		// Game
		{
			auto& gameObject = viewport.AddChild();
		
			auto& levelData = gameObject.AddComponent(tei::Resources->LoadUnique<LevelData>("", "versus"));
			auto& level = Level::Create(gameObject, levelData);

			Anima::Create(level.AddChild(), levelData->anima[0]).AddComponent<ObjectTransform>(unit::Position{ -.5f,  3.f });
			Anima::Create(level.AddChild(), levelData->anima[1]).AddComponent<ObjectTransform>(unit::Position{  1.f, -2.f });
			Anima::Create(level.AddChild(), levelData->anima.back()).AddComponent<ObjectTransform>(unit::Position{  1.f, 2.f });

			[[maybe_unused]] auto& score = ScoreDisplay::Create(level);

			auto offset = level.GetComponent<ObjectTransform>()->scale;
			score.AddComponent(ObjectTransform{ { unit::Position{ -.55f / offset.x, .45f / offset.y }, unit::Scale{.5f} } });

			score.AddComponent<Observed<unit::Colour>>(unit::Colour{ 1, 0, 0, 1 });
		}

		// Controls
		{
			viewport.AddComponent(
				std::tuple{
					tei::Input->AddCommand(
						tei::input::KeyboardInput::Main::ESCAPE,
						[](auto) mutable
						{
							tei::Application->Quit();
						}
					),
					tei::Input->AddCommand(
						tei::input::KeyboardInput::F::F11,
						[state = false](auto) mutable
						{
							using enum tei::application::WindowProperty;
							state ^= true;
							tei::Application->SetWindowProperty(state ? FULLSCREEN_FAKE : RESTORED);
						}
					)
				}
			);
		}
	}

	// FPS
	{
		auto& fps = scene.AddChild();
		fps.AddComponent(ObjectTransform{ { tei::unit::Scale{ 14 } } });
		fps.AddComponent(tei::Resources->LoadShared<tei::resource::Font>("resources/Lingua.otf", 11));
		fps.AddComponent(WindowAlignment{ { -14, -10 }, { 1, 1 } });
		fps.AddComponent<FPS>();
	}
}

void TeiClientCleanup()
{

}