#pragma once

#include <tei.h>

struct Sound
{

	using Type = std::string_view;
	constexpr static Type DEATH     = "death";
	constexpr static Type HIT       = "hit";
	constexpr static Type ATTACK    = "attack";
	constexpr static Type DROP      = "drop";
	constexpr static Type STEP      = "step";
	constexpr static Type CLICK     = "click";
	constexpr static Type PRESS     = "press";
	constexpr static Type CRUSH     = "crush";
	constexpr static Type COMPLETED = "completed";
	
	Type type;
	tei::Resource<tei::resource::Sound> sound;

};

