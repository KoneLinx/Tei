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
	int id{};
	bool flips{};
	bool hostile{};

};

class Anima : public tei::components::RefComponent<tei::components::ObjectTransform, tei::Resource<tei::resource::Sprite>>
{
public:

	static tei::ecs::Object& Create(tei::ecs::Object& object, AnimaData const& data);

	void OnEnable(tei::ecs::Object const&);
	void OnUpdate();
	void OnDisable();

	void DoAttack();
	void DoDeath();
	void DoHit();

	//void SetState(AnimaData::State::ID state, tei::Clock::duration time);
	
	void SetInput(tei::unit::Vec2 movement)
	{ m_Movement = movement; }

	std::pair<bool, bool> GetAllowedAxis() const
	{ return { m_AllowX > 0, m_AllowY > 0 }; }

	bool IsActive();

private:

	AnimaData const* m_pData{};

	AnimaData::State::ID m_State{};

	tei::unit::Vec2 m_Movement{};

	int m_AllowX{};
	int m_AllowY{};

	tei::time::TimeOnce m_Timer{};

	std::any m_Handles{};
};
