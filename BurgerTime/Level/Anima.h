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
			HIT,
			ATTACK_PARTICLE
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

struct Cloud{};

class Anima : public tei::components::RefComponent<tei::components::ObjectTransform, tei::Resource<tei::resource::Sprite>>
{
public:

	static tei::ecs::Object& Create(tei::ecs::Object& object, AnimaData const& data, tei::unit::Position spawnPoint);

	void OnEnable(tei::ecs::Object&);
	void OnUpdate();
	void OnDisable();

	void DoAttack();
	void DoDeath(bool stayDead = false);
	void DoHit();
	
	void SetInput(tei::unit::Vec2 movement)
	{ m_Movement = movement; }

	struct AllowedMovement
	{
		char right, left, up, down;
	};

	AllowedMovement GetAllowedMovement() const
	{ return m_AllowedMovement; }

	tei::unit::Vec2 GetMovement() const
	{ return m_Movement; }

	bool IsActive();
	bool IsAlive();

private:

	tei::ecs::Object* m_pParent{};
	tei::ecs::Object* m_pObject{};
	AnimaData const* m_pData{};

	AnimaData::State::ID m_State{};

	tei::unit::Position m_SpawnPoint{};

	tei::unit::Vec2 m_Movement{};

	AllowedMovement m_AllowedMovement;

	tei::time::TimeOnce m_Timer{};

	std::any m_Handles{};
};
