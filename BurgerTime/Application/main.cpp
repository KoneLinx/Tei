#include "pch.h"

#include "FPS.h"

#include "Menu.h"
#include "GameManager.h"

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
			viewport.AddComponent<GameManager>();
		}

		// Application controls
		{
			tei::Input->AddCommand(
				tei::input::KeyboardInput::F::F11,
				[state = false](auto) mutable
				{
					using enum tei::application::WindowProperty;
					state ^= true;
					tei::Application->SetWindowProperty(state ? FULLSCREEN_FAKE : RESTORED);
				}
			).Detach();
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