
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <ImGui.h>

#include <iostream>
#include <thread>

#include <tei/time.h>
#include <tei/unit.h>
#include <tei/application.h>
#include <tei/render.h>
#include <tei/audio.h>

int main(int argc, char* argv[])
{
	METRICS_INITLOG("_log.json");
	METRICS_TIMEBLOCK;

	tei::internal::application::StartApplication(argc, argv);

	tei::internal::resource::Resources.Register(new tei::internal::resource::ResourceManager{});
	tei::internal::resource::Resources->AddLoader(new tei::internal::resource::prefab::TextureLoader{});
	//auto& texture = tei::external::Resources->Load<tei::external::resource::Texture>("_test.jpg");

	tei::internal::audio::Audio.Register(new tei::internal::audio::DebugAudio{});

	bool running{ true };
	while (running)
	{
		SDL_Event e{};
		while (SDL_PollEvent(&e))
			if (e.type == SDL_EventType::SDL_QUIT)
				running = false;
			else if (e.type == SDL_EventType::SDL_KEYUP && e.key.keysym.scancode == SDL_Scancode::SDL_SCANCODE_F11)
				tei::external::Application->SetFullscreen(true);

		tei::external::Audio->Play(tei::internal::resource::Sound{ nullptr, "TestAudio" });
		tei::internal::audio::Audio->Update();

		tei::internal::time::Time->frame.now = tei::external::Clock::now();
		tei::internal::render::Renderer->Update();
		tei::internal::render::Renderer->Clear();
		//tei::external::Renderer->DrawTexture(texture, tei::external::unit::Transform{ {}, { .5f, .5f } });
		tei::internal::render::Renderer->Present();
	}

	return 0;
}
