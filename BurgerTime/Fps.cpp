#include "Fps.h"

tei::ecs::Object& CreateFps(tei::ecs::Object& parent)
{
	auto& fps = parent.AddChild();

	fps.AddComponent<tei::components::ObjectTransform>(tei::unit::Position{ tei::Renderer->GetRenderTraget().size / -2.f + 16.f});
	fps.AddComponent<FpsComponent>();
	fps.AddComponent<tei::components::Observed<std::string>>();
	fps.AddComponent<tei::components::ObserverComponent>();
	fps.AddComponent<tei::components::TextRenderComponent>();
	fps.AddComponent(tei::Resources->LoadUnique<tei::resource::Font>("resources/Lingua.otf", 12));
	fps.AddComponent(tei::Resources->LoadUnique<tei::resource::Texture>());

	return fps;
}

void OnEnable(FpsComponent& fps, tei::ecs::Object& object)
{
	auto& [text, transform] = fps.refs;
	object
		.GetComponent<tei::components::ObserverComponent>()
		.AddObserver(
			tei::Event,
			[&transform] (tei::application::WindowPropertyChangedEvent const&)
			{
				transform.get().position = tei::Application->GetWindow().transform.scale / -2.f + 16.f;
			}
		);
}

void OnUpdate(FpsComponent& fps)
{
	auto& [text, transform] = fps.refs;

	fps.average = (fps.average * 9 + tei::Time->frame.delta * 1) / 10;
	auto ntext = std::to_string(int(1 / fps.average + .25f));
	if (*text != ntext)
		text = std::move(ntext);
}