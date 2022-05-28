#pragma once

#include <tei.h>

#include "Anima.h"
#include "Score.h"

class EnemyController : public tei::components::RefComponent<Anima, tei::components::ObjectTransform>
{
public:

	EnemyController(AnimaData const& data);

	void OnEnable(tei::ecs::Object& object);
	void OnUpdate();
	void OnDisable();

private:

	AnimaData const* m_pData;

	Anima::AllowedMovement m_OldMovement;

	class Level const* m_Level;

};

class EnemyEffects : public tei::components::RefComponent<Anima, Score>
{
public:

	void OnEnable(tei::ecs::Object&);
	void OnDisable();

private:

	std::any m_Handles;

};