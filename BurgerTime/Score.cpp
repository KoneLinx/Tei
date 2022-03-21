#include "Score.h"
#include "Level.h"
#include "Achievements.h"

tei::ecs::Object& CreateScore(tei::ecs::Object& parent)
{
	auto& scoreObject = parent.AddChild();

	scoreObject.AddComponent<tei::components::ObjectTransform>(tei::unit::Position{ 0, -64 });
	scoreObject.AddComponent<Score>();
	scoreObject.AddComponent<ScoreDisplay>();
	
	scoreObject.AddComponent<tei::components::ObserverComponent>();

	scoreObject.AddComponent<tei::components::Observed<std::string>>();
	scoreObject.AddComponent<tei::components::TextRenderComponent>();
	scoreObject.AddComponent(tei::Resources->LoadShared<tei::resource::Font>("resources/Lingua.otf", 12));
	scoreObject.AddComponent(tei::Resources->LoadUnique<tei::resource::Texture>());

	return scoreObject;
}

void OnEnable(Score& score, tei::ecs::Object const& object)
{
	score.get() = 0;
	auto& subject = object.GetParent().GetComponent<tei::components::Subject>();
	auto& observers = object.GetComponent<tei::components::ObserverComponent>();
	observers.AddObserver(
		subject,
		[&score] (Player::Event event)
		{
			if (event.event == Player::Event::SCORE)
				score.get() += 73;
		}
	);
}

void OnUpdate(ScoreDisplay const& display, tei::ecs::Object const& object)
{
	auto& [text, score] = display.refs;
	if (score.checkout())
	{
		std::stringstream str{};
		str << "Score: " << score;
		text = str.str();

		if (score > 500)
		{
			tei::Event->Notify(Achievenments::FIRST_LEVEL_COMPETED);
			tei::Event->Notify(Level::Event::COMPLETED);
			object.GetComponent<tei::components::ObserverComponent>().Clear();
		}
	}
}
