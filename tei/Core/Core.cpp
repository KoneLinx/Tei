#include "Core.h"

#include <thread>

#include <tei/time.h>
#include <tei/scene.h>
#include <tei/audio.h>
#include <tei/render.h>

#include "../External/Client.h"

using namespace tei::internal::core;

using namespace tei::internal;
using namespace tei::internal::time;
using namespace tei::internal::time::literals;
using enum ecs::Message;

struct StopSignal {};

void Core::GameLoop()
{
	auto& time = Time->frame;
	auto current = time.now = Clock::now();
	while (m_IsRunning)
	{
		METRICS_TIMEBLOCK;

		Time->global.now = current = Clock::now();
		if (time.fixed)
		{
			time.lag = current - time.now;
			time.now += time.delta = time.step;

			FrameUpdate();

			if (m_IsRunning && time.lag < time.step)
				std::this_thread::sleep_until(time.now);
		}
		else
		{
			time.delta = current - time.now;
			time.now = current;
			time.lag = {};

			FrameUpdate();

			//if (m_IsRunning && time.synced)
			/* sync */
		}
	}
}

void Core::FrameUpdate()
{
	METRICS_TIMEBLOCK;

	// Update
	{
		//events::Events->Update();

		if (!m_IsRunning) return;

		scene::Scene->Do(UPDATE);

		if (!m_IsRunning) return;

		audio::Audio->Update();
		render::Renderer->Update();
	}

	// Render
	{
		render::Renderer->Clear();
		scene::Scene->Do(RENDER);
		render::Renderer->Present();
	}

	// debug
#if defined(DEBUG) || defined(_DEBUG)
	if (Time->global.now.time_since_epoch() > 3_s)
		Stop();
#endif // DEBUG

}

void Core::Run()
{
	METRICS_TIMEBLOCK;

	// Services
	audio::Audio.Register(new audio::SDLAudio{});
	//events::Events.Register(new events::EventManager{});
	resource::Resources.Register(new resource::ResourceManager{});
	scene::Scene.Register(new ecs::Object{ true });

	// Resource loaders
	resource::Resources->AddLoader(new resource::prefab::TextureLoader{});
	resource::Resources->AddLoader(new resource::prefab::FontsLoader{});
	resource::Resources->AddLoader(new resource::prefab::AudioLoader{});

	// Client init
	TeiClientInit();

	// Game
	try
	{
		GameLoop();
	}
	catch (StopSignal)
	{}

	// Clear scenes
	scene::Scene.Register(nullptr);

	// Client quit
	TeiClientCleanup();

	// Clear services
	resource::Resources.Register(nullptr);
	//events::Events.Register(nullptr);
	audio::Audio.Register(nullptr);

	// End
}

void Core::Stop()
{
	m_IsRunning = false;
	throw StopSignal{};
}