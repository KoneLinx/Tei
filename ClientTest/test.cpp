
#include <tei.h>

#include <iostream>
#include <sstream>

using namespace tei::literals;

struct Timer {};

void OnUpdate(Timer)
{
	if (tei::Time->frame.now > 5_s)
		tei::Application->Quit();
}

//void OnRender(Timer)
//{
//	std::cout << "  " << tei::Time->frame.now << '\r';
//}

//void OnUpdate(tei::components::TextureRenderComponent& t)
//{
//	get<tei::components::ObjectTransform>(t).local.rotation.r = std::sin(tei::Time->frame.now);
//}

struct FpsComponent : tei::components::RefComponent<"fps", tei::components::Observed<std::string>, tei::components::ObjectTransform>
{
	tei::Clock::duration average{ 1_s };
};

void OnUpdate(FpsComponent& fps)
{
	auto& [text, transform] = fps.refs;

	transform.local.position = tei::Application->GetWindow().transform.scale / -2.f + 16.f;

	std::stringstream out{};
	out << "[Frame]\tdelta:\t" << std::chrono::milliseconds{tei::Time->thread->delta}
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
	out << "[Fixed]\tdelta:\t" << std::chrono::milliseconds{tei::Time->thread->delta}
		<< "\tlag:\t" << std::chrono::microseconds{tei::Time->thread->lag}
		<< std::endl;
	std::cout << out.view();
	std::cout.flush();
}

void TeiClientInit()
{
	using namespace tei::components;
	using namespace tei::resource;
	using namespace tei::unit;

	tei::Application->SetWindowProperty(tei::application::WindowProperty::RESIZABLE);

	auto& scene{ tei::Scenes->AddScene(true) };

	scene.AddComponent<Timer>();

// Excersise week 1
#if true
	{

		auto& root = scene.AddChild();

		Scale windowScale{};

		{
			auto& bg = root.AddChild();

			auto& bgTex = bg.AddComponent(tei::Resources->LoadShared<Texture>("resources/background.jpg"));
			
			windowScale = bgTex->size;
			tei::Application->SetWindowProperty(windowScale);

			bg.AddComponent<ObjectTransform>();
			bg.AddComponent<TextureRenderComponent>();

			{
				auto& bglogo = bg.AddChild();

				bglogo.AddComponent(tei::Resources->LoadShared<Texture>("resources/logo.png"));
				bglogo.AddComponent<ObjectTransform>();
				bglogo.AddComponent<TextureRenderComponent>();
			}
		}

		{
			auto& title = root.AddChild();

			title.AddComponent<tei::components::Observed<std::string>>({ "Splash screen text" });
			title.AddComponent(tei::Resources->LoadUnique<Font>("resources/Lingua.otf", 32));
			title.AddComponent(tei::Resources->LoadUnique<Texture>("", 0));
			title.AddComponent<ObjectTransform>(Position{ 0, -windowScale.y / 2.f / 5.f * 3.f });
			title.AddComponent<TextRenderComponent>();
		}

		{
			auto& fps = scene.AddChild();

			fps.AddComponent<tei::components::Observed<std::string>>();
			fps.AddComponent(tei::Resources->LoadUnique<Font>("resources/Lingua.otf", 11));
			fps.AddComponent(tei::Resources->LoadUnique<Texture>("", 0));
			fps.AddComponent<ObjectTransform>(Position{ windowScale / -2.f + 12.f });
			fps.AddComponent<FpsComponent>();
			fps.AddComponent<TextRenderComponent>();
		}

#endif

	}

}

void TeiClientCleanup()
{

}