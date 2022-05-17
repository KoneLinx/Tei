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
		};
		using enum ID;
		std::vector<tei::Resource<tei::resource::Sprite>> sprites{};
	}
	state{};

	long score{};
	bool flips{};

};

class Anima : tei::components::RefComponent<tei::components::ObjectTransform, tei::Resource<tei::resource::Sprite>>
{
public:

	tei::ecs::Object& Create(tei::ecs::Object& object, AnimaData const& data);

	void OnEnable(tei::ecs::Object const&);
	void OnUpdate();
	void OnDisable();

private:

	AnimaData const* m_pData{};

	AnimaData::State::ID m_State{};

	tei::unit::Vec2 m_Movement{};

	tei::unit::Vec2 m_AllowedMovement{};

	tei::time::TimeOnce m_Timer{};

	std::any m_Storage{};
};

