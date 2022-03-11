
#include <tei.h>

#include <iostream>

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

struct FpsComponent : tei::components::RefComponent<"fps", tei::components::Observed<std::string>>
{
	tei::Clock::duration average{ 1_s };
};

void OnUpdate(FpsComponent& fps)
{
	std::cout << std::chrono::milliseconds{ tei::Time->frame.delta } << '\t' << std::chrono::microseconds{ tei::Time->frame.lag } << std::endl;
	auto& [text] = fps.refs;
	fps.average = (fps.average * 59 + tei::Time->frame.delta * 1) / 60;
	auto ntext = std::to_string(int(1 / fps.average + .25f));
	if (*text != ntext)
		text = std::move(ntext);
}

void TeiClientInit()
{
	using namespace tei::components;
	using namespace tei::resource;
	using namespace tei::unit;

	tei::Scene->AddComponent<Timer>();

// Excersise week 1
#if true
	{

		auto& root = tei::Scene->AddChild();

		{
			auto& bg = root.AddChild();

			auto& bgTex = bg.AddComponent(tei::Resources->LoadShared<Texture>("resources/background.jpg"));
			tei::Application->SetWindowSize(bgTex->w, bgTex->h);

			bg.AddComponent<ObjectTransform>({ Scale{ 2, 2 } });
			bg.AddComponent<TextureRenderComponent>();

			{
				auto& bglogo = bg.AddChild();

				bglogo.AddComponent(tei::Resources->LoadShared<Texture>("resources/logo.png"));
				bglogo.AddComponent<ObjectTransform>({ Scale{ .2f, .2f } });
				bglogo.AddComponent<TextureRenderComponent>();
			}
		}


		{
			auto& title = root.AddChild();

			title.AddComponent<tei::components::Observed<std::string>>({ "Splash screen text" });
			title.AddComponent(tei::Resources->LoadUnique<Font>("resources/Lingua.otf", 32));
			title.AddComponent(tei::Resources->LoadUnique<Texture>("", 0));
			title.AddComponent<ObjectTransform>({ { Position{ 0, -.66f }, Scale{ .2f, .2f } } });
			title.AddComponent<TextRenderComponent>();
		}
	}
#endif

	{
		auto& fps = tei::Scene->AddChild();

		fps.AddComponent<tei::components::Observed<std::string>>();
		fps.AddComponent(tei::Resources->LoadUnique<Font>("resources/Lingua.otf", 11));
		fps.AddComponent(tei::Resources->LoadUnique<Texture>("", 0));
		fps.AddComponent<ObjectTransform>({ { Position{ -.95f, -.95f }, Scale{ .05f, .05f } } });
		fps.AddComponent<FpsComponent>();
		fps.AddComponent<TextRenderComponent>();
	}

	auto& scene = *tei::Scene.operator->();
}

void TeiClientCleanup()
{

}