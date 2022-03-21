#pragma once

#include <tei.h>

struct Player
{

	struct Event
	{
		enum struct ID
		{
			SPAWN,
			DEATH,
			SCORE //tmp
		};

		using enum ID;

		ID event;
		Player const* player;
	};

	int playerid;
	int lives;
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

tei::ecs::Object& CreatePlayer(tei::ecs::Object&, int playerid);
tei::ecs::Object& CreatePlayerTracker(tei::ecs::Object&);
tei::ecs::Object& CreatePlayerInfo(tei::ecs::Object&, int playerid);

void OnInitialize(PlayerTracker&, tei::ecs::Object&);
void OnInitialize(PlayerInfo&, tei::ecs::Object&);

void OnEnable(Player&, tei::ecs::Object&);
void OnEnable(PlayerInfo&, tei::ecs::Object&);