#include "teipch.h"
#include "Core.h"

#include <thread>
#include <latch>

#include <tei.h>

#include <SDL.h>
#include <ImGui.h>


using namespace tei::internal::core;

using namespace tei::internal;
using namespace tei::literals;

utility::Static<CoreFunction> tei::internal::core::Core{};

enum struct FrameUpdateType : size_t
{
	RENDER        = 1ull << 0,
	FREE_UPDATE   = 1ull << 1,
	FIXED_UPDATE  = 1ull << 2,
	EVENTS        = 1ull << 3,

	FRAME         = RENDER | EVENTS | FREE_UPDATE,
	FIXED         = FIXED_UPDATE,
};

bool operator == (FrameUpdateType a, FrameUpdateType b)
{
	using Int = std::underlying_type_t<FrameUpdateType>;
	return (Int(a) & Int(b)) != 0;
}


void FrameUpdate(bool& isRunning, FrameUpdateType updateType)
{
	METRICS_TIMEBLOCK;

	if (isRunning && updateType == FrameUpdateType::EVENTS)
	{
		application::ApplicationService->Update();
		input::Input->ProcessInput();
	}
	if (isRunning && updateType == FrameUpdateType::FREE_UPDATE)
	{
		scene::Scenes->Do(ecs::Message::UPDATE);
		audio::Audio->Update();
	}
	if (isRunning && updateType == FrameUpdateType::FIXED_UPDATE)
	{
		scene::Scenes->Do(ecs::Message::FIXEDUPDATE);
	}
	if (isRunning && updateType == FrameUpdateType::RENDER)
	{
		render::Renderer->Update();
		render::Renderer->Clear();
		scene::Scenes->Do(ecs::Message::RENDER);
		render::Renderer->Present();
	}

}

void GameLoop(bool& isRunning, auto& time, FrameUpdateType updateType)
{
	//auto& time = time::Time->frame;
	auto current = time.now = time::Clock::now();

	while (isRunning)
	{
		METRICS_TIMEBLOCK;

		time::Time->global.now = current = time::Clock::now();

		if (time.fixed)
		{
			time.lag = current - time.now;
			time.now += time.delta = time.step;

			if (!time.allow_drop || time.lag < time.step)
				FrameUpdate(isRunning, updateType);

			if (isRunning && time.lag < time.step)
				std::this_thread::sleep_until(time.now);
		}
		else
		{
			time.lag = {};
			time.delta = current - time.now;
			time.now = current;

			FrameUpdate(isRunning, updateType);
		}
	}
}

void CoreFunction::Run()
{
	METRICS_TIMEBLOCK;

	bool isRunning{ true };
	m_IsRunning = &isRunning;

	// Services
	resource::Resources.Register(new resource::ResourceManager{});
	audio::Audio.Register(new audio::SDLAudio{});
	input::Input.Register(new input::InputManager{});
	events::Event.Register(new components::Subject{});
	scene::Scenes.Register(new scene::SceneManager{});

	//std::this_thread::sleep_for(1.5_s);
	application::ApplicationService->SetWindowProperty(application::WindowProperty::BORDERED);

	// Client init
	TeiClientInit();

	// Game
	{
		std::latch sync{ 2 };

		//std::jthread fixed{
		//	[&isRunning, &sync]
		//	{
		//		METRICS_TIMEBLOCK;
		//		time::Time->thread = &time::Time->fixed;
		//		sync.arrive_and_wait();
		//		GameLoop(isRunning, time::Time->fixed, FrameUpdateType::FIXED);
		//	}
		//};
		
		// render
		[&isRunning, &sync]
		{
			METRICS_TIMEBLOCK;
			//sync.arrive_and_wait();
			time::Time->thread = &time::Time->frame;
			GameLoop(isRunning, time::Time->frame, FrameUpdateType::FRAME);
		}
		();
	}

	// Clear scenes
	scene::Scenes.Register(nullptr);

	// Client quit
	TeiClientCleanup();

	// Clear services
	resource::Resources.Register(nullptr);
	input::Input.Register(nullptr);
	events::Event.Register(nullptr);
	audio::Audio.Register(nullptr);

	// End
}

void CoreFunction::Stop()
{
	*m_IsRunning = false;
}