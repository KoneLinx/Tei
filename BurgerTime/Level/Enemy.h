#pragma once

#include <tei.h>

#include "Anima.h"
#include "Score.h"

class EnemyController : public tei::components::RefComponent<Anima>
{
public:

	EnemyController(AnimaData const& data);

	void OnEnable();
	void OnUpdate();
	void OnDisable();

private:

	AnimaData const* m_pData;

	std::any m_Handles;

};

class EnemyEffects : public tei::components::RefComponent<Anima, Score>
{
public:

	void OnEnable(tei::ecs::Object&);
	void OnDisable();

private:

	std::any m_Handles;

};