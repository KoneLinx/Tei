#pragma once

#include <tei.h>

struct Achievenments
{

	enum struct Event
	{
		FIRST_LEVEL_COMPETED
	};
	using enum Event;

};

tei::ecs::Object& CreateAchievements(tei::ecs::Object&);

void OnEnable(Achievenments const&);
