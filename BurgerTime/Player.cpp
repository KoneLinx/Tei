#include "Player.h"
#include "Level.h"

using namespace tei;

ecs::Object& CreatePlayer(ecs::Object& parent, int id)
{
	auto& playerObject = parent.AddChild();

	playerObject.AddComponent<components::ObjectTransform>(tei::unit::Position{ 0, 64 });
	playerObject.AddComponent<components::InputComponent>();
	playerObject.AddComponent<components::Subject>();

	auto& player = playerObject.AddComponent<Player>();
	player.playerid = id;
	player.lives = 3;

	return playerObject;
}

tei::ecs::Object& CreatePlayerTracker(tei::ecs::Object& parent)
{
	auto& trackObject = parent.AddChild();
	
	trackObject.AddComponent<PlayerTracker>();
	trackObject.AddComponent<tei::components::ObserverComponent>();

	return trackObject;
}

tei::ecs::Object& CreatePlayerInfo(tei::ecs::Object& parent, int id)
{
	auto& infoObject = parent.AddChild();

	infoObject.AddComponent<PlayerInfo>().playerid = id;
	infoObject.AddComponent<tei::components::ObserverComponent>();

	return infoObject;
}

void OnEnable(Player& player, ecs::Object& object)
{
	auto& input = object.GetComponent<components::InputComponent>();
	auto& subject = object.GetComponent<components::Subject>();
	input.AddCommand(
		player.playerid ? tei::input::KeyboardInput::Arrow::DOWN : tei::input::KeyboardInput::Home::PAGEDOWN,
		[&]
		{
			--player.lives;
			subject.Notify<Player::Event>({
				Player::Event::DEATH,
				&player
			});
			if (player.lives == 0)
			{
                puts("Player died!");
				tei::Event->Notify(Level::Event::FAILED);
				object.SetState(false);
			}
		}
	);
	input.AddCommand(
		player.playerid ? tei::input::KeyboardInput::Arrow::UP : tei::input::KeyboardInput::Home::PAGEUP,
		[&]
		{
			subject.Notify<Player::Event>({ 
				Player::Event::SCORE, 
				&player
			});
		}
	);
	subject.Notify<Player::Event>({
		Player::Event::SPAWN,
		&player
	});
}

void OnInitialize(PlayerTracker& tracker, tei::ecs::Object& object)
{
	auto& parent = object.GetParent();

	auto& trackObservers = object.AddComponent<tei::components::ObserverComponent>();
	auto& levelSubject = parent.GetComponent<tei::components::Subject>();

	trackObservers.AddObserver(
		levelSubject,
		[&](Player::Event event)
		{
			if (event.event == Player::Event::SPAWN)
			{
				++tracker.playerAliveCount;
				CreatePlayerInfo(parent, event.player->playerid);
			}
		}
	);
	trackObservers.AddObserver(
		levelSubject,
		[&](Player::Event event)
		{
			if (event.event == Player::Event::DEATH && event.player->lives == 0)
			{
				if (--tracker.playerAliveCount == 0)
					levelSubject.Notify(Level::Event::FAILED);
			}
		}
	);
}

void OnInitialize(PlayerInfo& info, tei::ecs::Object& object)
{
	auto& scoreDisplay = object.AddChild();
	auto& livesDisplay = object.AddChild();
	
	scoreDisplay.AddComponent<tei::components::ObjectTransform>(tei::unit::Position{ 0, info.playerid * 96 + 0 });
	auto& scoreText = scoreDisplay.AddComponent<tei::components::Observed<std::string>>();
	scoreDisplay.AddComponent<tei::components::TextRenderComponent>();
	scoreDisplay.AddComponent(tei::Resources->LoadShared<tei::resource::Font>("resources/Lingua.otf", 12));
	scoreDisplay.AddComponent(tei::Resources->LoadUnique<tei::resource::Texture>());
	
	livesDisplay.AddComponent<tei::components::ObjectTransform>(tei::unit::Position{ 0, info.playerid * 96 + 16 });
	auto& livesText = livesDisplay.AddComponent<tei::components::Observed<std::string>>();
	livesDisplay.AddComponent<tei::components::TextRenderComponent>();
	livesDisplay.AddComponent(tei::Resources->LoadShared<tei::resource::Font>("resources/Lingua.otf", 12));
	livesDisplay.AddComponent(tei::Resources->LoadUnique<tei::resource::Texture>());

	auto& parent = object.GetParent();

	auto& infoObservers = object.AddComponent<tei::components::ObserverComponent>();
	auto& levelSubject = parent.GetComponent<tei::components::Subject>();

	auto const makeLivesText = [] (int lives)
	{
		std::stringstream str{};
		str << "Lives: " << lives;
		return str.str();
	};
	auto const makeScoreText = [] (int score)
	{
		std::stringstream str{};
		str << "Score: " << score;
		return str.str();
	};


	infoObservers.AddObserver(
		levelSubject,
		[&] (Player::Event event)
		{
			if (event.player->playerid != info.playerid)
				return;
			else
			if (event.event == Player::Event::DEATH)
				livesText = makeLivesText(event.player->lives);
			else
			if (event.event == Player::Event::SCORE)
			{
				scoreText = makeScoreText(info.score += 73);
				if (info.score >= 500)
					levelSubject.Notify(Level::Event::COMPLETED);
			}
		}
	);

	livesText = makeLivesText(3);
	scoreText = makeScoreText(0);
}

void OnEnable(PlayerInfo& info, tei::ecs::Object& object)
{}
