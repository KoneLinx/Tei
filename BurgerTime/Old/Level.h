#pragma once

#include <tei.h>
#include <nlohmann/json.hpp>

class Level
{

	Level();

public:

	static tei::ecs::Object& Create(tei::ecs::Object& parent, nlohmann::json const& object, bool versusMode = false);

	void OnEnable(tei::ecs::Object const& object);

	enum struct Event
	{
		SETUP,
		COMPLETED,
		FAILED,
	};

private:

	struct 
	{
		int stage{ 0 };
		int seed{ 0 };
		int difficulty{ 1 };
		int width{ 1 };
		int hieght{ 3 };
		int ladders{ 2 };
		int enemies{ 1 };
		int ingredients{ 1 };
		bool versus{ false };
	} 
	m_Leveldata{};

	bool completed{};


};

tei::ecs::Object& CreateLevel(tei::ecs::Object&);

//void OnEnable(Level&, tei::ecs::Object const&);