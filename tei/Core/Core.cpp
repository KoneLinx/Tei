#include "Core.h"

#include <thread>

#include <tei.h>

#include <SDL.h>

#include <ImGui.h>

using namespace tei::internal::core;

using namespace tei::internal;
using namespace tei::literals;

utility::Static<CoreFunction> tei::internal::core::Core{};

void CoreFunction::GameLoop()
{
	auto& frame = time::Time->frame;
	auto current = frame.now = Clock::now();
	while (m_IsRunning)
	{
		METRICS_TIMEBLOCK;

		time::Time->global.now = current = Clock::now();
		if (frame.fixed)
		{
			frame.lag = current - frame.now;
			frame.now += frame.delta = frame.step;

			FrameUpdate();

			if (m_IsRunning && frame.lag < frame.step)
				std::this_thread::sleep_until(frame.now);
		}
		else
		{
			frame.delta = current - frame.now;
			frame.now = current;
			frame.lag = {};

			FrameUpdate();

			//if (m_IsRunning && frame.synced)
			/* sync */
		}
	}
}

void CoreFunction::FrameUpdate()
{
	METRICS_TIMEBLOCK;

	// Update
	{

		SDL_Event e{};
		while (SDL_PollEvent(&e))
		{
			ImGui_ImplSDL2_ProcessEvent(&e);
			if (e.type == SDL_QUIT)
				Stop();
		}

		input::Input->ProcessInput();
		//events::Events->Update();

		if (!m_IsRunning) return;

		scene::Scene->Do(ecs::Message::UPDATE);

		if (!m_IsRunning) return;

		audio::Audio->Update();
		render::Renderer->Update();
	}

	// Render
	{
		render::Renderer->Clear();
		scene::Scene->Do(ecs::Message::RENDER);
		render::Renderer->Present();
	}

}

void CoreFunction::Run()
{
	METRICS_TIMEBLOCK;

	// Services
	resource::Resources.Register(new resource::ResourceManager{});
	audio::Audio.Register(new audio::SDLAudio{});
	input::Input.Register(new input::InputManager{});
	//events::Events.Register(new events::EventManager{});
	scene::Scene.Register(new ecs::Object{ ecs::CreateRoot() });

	// Resource loaders
	resource::Resources->AddLoader(new resource::prefab::TextureLoader{});
	resource::Resources->AddLoader(new resource::prefab::FontsLoader{});
	resource::Resources->AddLoader(new resource::prefab::AudioLoader{});

	std::this_thread::sleep_for(1.5_s);
	application::ApplicationService->SetWindowBorder(true);

	// Client init
	TeiClientInit();

	// Game
	GameLoop();

	// Clear scenes
	scene::Scene.Register(nullptr);

	// Client quit
	TeiClientCleanup();

	// Clear services
	resource::Resources.Register(nullptr);
	input::Input.Register(nullptr);
	//events::Events.Register(nullptr);
	audio::Audio.Register(nullptr);

	// End
}

void CoreFunction::Stop()
{
	m_IsRunning = false;
}