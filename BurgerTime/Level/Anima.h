#pragma once

#include <tei.h>

struct AnimaData
{
	using Type = std::string_view;

	static inline constexpr Type PLAYER = "player";
	static inline constexpr Type ENEMY  = "enemy";

	Type type;
	tei::unit::Dimentions box;

	std::string name{};
	
	struct State
	{
		enum struct ID : size_t
		{
			STATIONARY,
			WALKING_LEFT,
			WALKING_RIGHT,
			WALKING_DOWN,
			WALKING_UP,
			ATTACKING,
			DYING,
			HIT
		} 
		state{};
		using enum ID;
		std::vector<tei::Resource<tei::resource::Sprite>> sprites{};
	}
	state{};

	long score{};
	bool flips{};

};

class Anima
{
};

