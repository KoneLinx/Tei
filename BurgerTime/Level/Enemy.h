#pragma once

#include <tei.h>

#include "Anima.h"

class EnemyController : public tei::components::RefComponent<Anima>
{
public:

	EnemyController(AnimaData const& data);

	void OnEnable(tei::ecs::Object&);
	void OnUpdate();
	void OnDisable();

private:

	AnimaData const* m_pData;

	std::any m_Handles;

};

