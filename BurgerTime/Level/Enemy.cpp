#include "pch.h"
#include "Enemy.h"
#include "Player.h"
#include "Hitbox.h"
#include "Ingredient.h"
#include "Score.h"

EnemyController::EnemyController(AnimaData const& data)
	: m_pData{ &data }
{}

void EnemyController::OnEnable()
{}

void EnemyController::OnUpdate()
{}

void EnemyController::OnDisable()
{
	m_Handles = {};
}

void EnemyEffects::OnEnable(tei::ecs::Object& object)
{
	auto& [anima, score] = Refs();

	auto makeHandle = [&]
	{
		return object.GetComponent<Hitbox>().AddObserver(
			[&](Hitbox::Hit const& hit)
			{
				
				if (hit.state != hit.ENTER)
					return;
				
				else if (hit.object.HasComponent<int/*attack cloud*/>())
				{
					anima.DoHit();
				}
			}
		);
	};

	m_Handles = std::make_shared<decltype(makeHandle())>(makeHandle());
}

void EnemyEffects::OnDisable()
{
	m_Handles = {};
}
