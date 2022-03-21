#pragma once

#include <tei.h>

struct Level
{

	enum struct Event
	{
		COMPLETED,
		FAILED,
	};

	bool completed{};

};

tei::ecs::Object& CreateLevel(tei::ecs::Object&);

//void OnEnable(Level&, tei::ecs::Object const&);