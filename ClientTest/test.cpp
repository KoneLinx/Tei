#include "pch.h"

#include <iostream>
#include <sstream>
#include <random>

using namespace tei::literals;

struct Timer {};

void OnUpdate(Timer, tei::ecs::Object const& object)
{
	static tei::time::TimeTimes timer{ 1_s, 5, tei::Time->frame.now + 3_s };
	static tei::time::TimeTimes timer2{ 1_s, 5, tei::Time->frame.now + 2_s + 100_ms  };
	if (timer || timer2)
	{
		if (auto* p{ object.HasComponent<tei::Resource<tei::resource::Sound>>() })
			tei::Audio->Play(*p);
	}

	if (tei::Time->frame.now > 5_s)
		0;
		//tei::Application->Quit();
}

void OnRender(Timer)
{
	std::cout << "  " << tei::Time->frame.now << '\r';
}

struct FpsComponent : tei::components::RefComponent<tei::components::Observed<std::string>, tei::components::ObjectTransform>
{
	tei::Clock::duration average{ 1_s };
};

void OnUpdate(FpsComponent& fps)
{
	auto& [text, transform] = fps.refs;

	transform.get().position = tei::Application->GetWindow().transform.scale / -2.f + 16.f;

	std::stringstream out{};
	out << "                \r[Frame]\tdelta:\t" << std::chrono::milliseconds{tei::Time->thread->delta}
		<< "\tlag:\t" << std::chrono::microseconds{tei::Time->thread->lag}
		<< std::endl;
	std::cout << out.view();
	std::cout.flush();

	fps.average = (fps.average * 9 + tei::Time->frame.delta * 1) / 10;
	auto ntext = std::to_string(int(1 / fps.average + .25f));
	if (*text != ntext)
		text = std::move(ntext);
}

void OnFixedUpdate(FpsComponent const&)
{
	std::stringstream out{};
	out << "                \r[Fixed]\tdelta:\t" << std::chrono::milliseconds{tei::Time->thread->delta}
		<< "\tlag:\t" << std::chrono::microseconds{tei::Time->thread->lag}
		<< std::endl;
	std::cout << out.view();
	std::cout.flush();
}

struct RootComponent : tei::components::RefComponent<tei::components::ObjectTransform>
{};

void OnUpdate(RootComponent& comp)
{
	auto& [transform] = comp.refs;
	float scale{ 1 + std::sin(tei::Time->thread->now.count() * 2) / 5 };
	transform.get().rotation.r = scale - 1;
	transform.get().scale = { scale, scale };
}

struct SplashText : tei::components::RefComponent<tei::components::Observed<std::string>>
{};

void OnUpdate(SplashText& comp)
{
	auto& [text] = comp.refs;
	text.get() = (std::stringstream{} << "0x" << std::hex << std::random_device()()).view();
}

void TeiClientInit()
{
	using namespace tei::components;
	using namespace tei::resource;
	using namespace tei::unit;
	
	//tei::Application->SetWindowProperty(tei::application::WindowProperty::FULLSCREEN_FAKE);

	auto& scene{ tei::Scenes->AddScene() };

	scene.AddComponent<Timer>();

	{

		auto& root = scene.AddChild();

		Scale windowScale{};

		root.AddComponent<ObjectTransform>(Scale{ 2 });
		root.AddComponent<RootComponent>();

		{
			auto& bg = root.AddChild();

			auto& bgTex = bg.AddComponent(tei::Resources->LoadShared<Texture>("resources/background.jpg"));
			
			windowScale = bgTex->size;
			tei::Application->SetWindowProperty(Scale{ windowScale * 2.f });

			bg.AddComponent<TextureRenderComponent>();

			{
				auto& bglogo = bg.AddChild();

				bglogo.AddComponent(tei::Resources->LoadShared<Texture>("resources/logo.png"));
				bglogo.AddComponent<TextureRenderComponent>();
			}
		}

		{
			auto& title = root.AddChild();

			title.AddComponent(tei::Resources->LoadUnique<Font>("resources/Lingua.otf", 32));
			title.AddComponent<ObjectTransform>(Position{ 0, -windowScale.y / 2 / 5 * 3 });
			title.AddComponent<SplashText>();
			title.AddComponent<TextRenderComponent>();
		}

		{
			auto& fps = scene.AddChild();

			fps.AddComponent(tei::Resources->LoadUnique<Font>("resources/Lingua.otf", 11));
			fps.AddComponent<ObjectTransform>(Position{ windowScale / -2.f + 12.f });
			fps.AddComponent<FpsComponent>();
			fps.AddComponent<TextRenderComponent>();
		}

	}

	tei::Input->AddCommand(
		tei::input::KeyboardInput::Main::ESCAPE,
		[]
		{ 
			tei::Application->Quit(); 
		}
	).Detach();

	scene.AddComponent(tei::Resources->LoadUnique<tei::resource::Sound>("resources/spin.mp3"));

	tei::Scenes->SetSceneState(scene, true);

}

void TeiClientCleanup()
{

}