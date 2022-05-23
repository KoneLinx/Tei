#pragma once

#include <tei.h>

#include "Anima.h"

class PlayerController : public tei::components::RefComponent<Anima>
{
public:

	PlayerController(AnimaData const& data);

	void OnEnable(tei::ecs::Object const& object);
	void OnDisable();

private:

	int m_playerID;

	tei::unit::Vec2 m_Movement{};

	std::any m_Handles{};

};

enum struct PlayerEvent
{
	DEATH,
	ATTACK
};

class PlayerEffects : public tei::components::RefComponent<Anima>
{
public:

	void OnEnable(tei::ecs::Object&);
	void OnDisable();

private:

	std::any m_Handles;

};

class PlayerAttackCount : public tei::components::RefComponent<tei::components::Observed<std::string>>
{
public:

	static tei::ecs::Object& Create(tei::ecs::Object& parent);

};