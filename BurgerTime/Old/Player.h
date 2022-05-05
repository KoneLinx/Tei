#pragma once

#include <tei.h>

using namespace tei::common;

class Player
{
	struct _Event;
public:

	using Event = _Event;

	Player(int playerID);

	tei::ecs::Object& Create(tei::ecs::Object& parent, int id);

	void OnInitialize(Object&);
	void OnEnable();
	void OnDisable();

	void TakeDamage();
	void Score();

	int GetID() const noexcept
	{ return m_Playerid; }

	tei::components::Subject& GetSubject()
	{ return m_Subject;	}

private:

	tei::components::Subject m_Subject;
	tei::components::InputComponent m_Input;
	int m_Playerid;
	int m_Lives;
	Object* m_pParent;
	tei::components::ObjectTransform* m_pTransform;

	struct _Event
	{
		enum struct ID
		{
			SPAWN,
			DAMAGED,
			DEATH,
			SCORE //tmp
		};

		ID event;
		Player const* player;
	};
	
	using enum Player::Event::ID;

	void Notify(Event::ID);

};

struct PlayerInfo
{
	int playerid;
	int score;
};

struct PlayerTracker 
{
	int playerAliveCount;
};

//tei::ecs::Object& CreatePlayer(tei::ecs::Object&, int playerid);
tei::ecs::Object& CreatePlayerTracker(tei::ecs::Object&);
tei::ecs::Object& CreatePlayerInfo(tei::ecs::Object&, int playerid);

void OnInitialize(PlayerTracker&, tei::ecs::Object&);
void OnInitialize(PlayerInfo&, tei::ecs::Object&);

//void OnEnable(Player&, tei::ecs::Object&);
void OnEnable(PlayerInfo&, tei::ecs::Object&);