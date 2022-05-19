#pragma once

#include <tei.h>

#include "Anima.h"

class PlayerController : public tei::components::RefComponent<Anima>
{
public:

	PlayerController(AnimaData const& data);

	void OnEnable(tei::ecs::Object&);
	void OnDisable();

private:

	int m_playerID;

	tei::unit::Vec2 m_Movement{};

	std::any m_Handles{};

};

