#include "Player.h"
#include "Level.h"

using namespace tei;
using namespace tei::components;

Player::Player(int id)
	: m_Playerid{ id }
	, m_Lives{ 3 }
	, m_Subject{}
	, m_Input{}
	, m_pParent{}
{}

tei::ecs::Object& Player::Create(tei::ecs::Object& parent, int id)
{
	auto& playerObject = parent.AddChild();

	auto& player = playerObject.AddComponent(Player{ id });
	m_pTransform = &playerObject.AddComponent(ObjectTransform{});
}

void Player::OnInitialize(Object& object)
{
	m_pParent = &object;
	auto [transform, input] = object.AddComponents(
		ObjectTransform{ unit::Position{0, 64} },
		InputComponent{}
	);
}

void Player::OnEnable()
{
	m_Input.AddCommand(
		m_Playerid ? tei::input::KeyboardInput::Arrow::DOWN : tei::input::KeyboardInput::Home::PAGEDOWN,
		[this] { TakeDamage(); }
	);
	m_Input.AddCommand(
		m_Playerid ? tei::input::KeyboardInput::Arrow::UP : tei::input::KeyboardInput::Home::PAGEUP,
		[this] { Score(); }
	);
	Notify(SPAWN);
}

void Player::OnDisable()
{
	m_Input.Clear();
}

void Player::TakeDamage()
{
	--m_Lives;
	Notify(DAMAGED);
	if (m_Lives == 0)
	{
		Notify(DEATH);
        puts("Player died!");
		m_pParent->SetState(false);
	}
}

typename void Player::Notify(Event::ID id)
{
	m_Subject.Notify(Event{ id, this });
}

void Player::Score()
{
	Notify(SCORE);
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



void OnInitialize(PlayerTracker& tracker, tei::ecs::Object& object)
{
	auto& parent = object.GetParent();

	auto& trackObservers = object.AddComponent<tei::components::ObserverComponent>();
	auto& levelSubject = parent.GetComponent<tei::components::Subject>();

	trackObservers.AddObserver(
		levelSubject,
		[&](Player::Event event)
		{
			if (event.event == Player::SPAWN)
			{
				++tracker.playerAliveCount;
				CreatePlayerInfo(parent, event.player->GetID());
			}
		}
	);
	trackObservers.AddObserver(
		levelSubject,
		[&](Player::Event event)
		{
			if (event.event == Player::DEATH)
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
	
	auto font = tei::Resources->LoadShared<tei::resource::Font>("resources/Lingua.otf", 12);
	scoreDisplay.AddComponents(
		ObjectTransform{ tei::unit::Position{ 0, info.playerid * 96 + 0 } },
		TextRenderComponent{},
		font
	);
	auto& scoreText = scoreDisplay.AddComponent<tei::components::Observed<std::string>>();
	
	scoreDisplay.AddComponents(
		ObjectTransform{ tei::unit::Position{ 0, info.playerid * 96 + 16 } },
		TextRenderComponent{},
		font
	);
	auto& livesText = livesDisplay.AddComponent<tei::components::Observed<std::string>>();

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
			if (event.player->GetID() != info.playerid)
				return;
			else
			if (event.event == Player::DAMAGED)
				livesText = makeLivesText(3);
			else
			if (event.event == Player::SCORE)
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
