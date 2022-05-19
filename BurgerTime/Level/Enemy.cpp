#include "pch.h"
#include "Enemy.h"
#include "Player.h"
#include "Hitbox.h"
#include "Ingredient.h"

EnemyController::EnemyController(AnimaData const& data)
	: m_pData{ &data }
{}

void EnemyController::OnEnable(tei::ecs::Object& object)
{
	auto& [anima] = Refs();
	auto& hitbox = object.GetComponent<Hitbox>();

	auto makeHandle = [&]
	{
		return hitbox.AddObserver(
			[&](Hitbox::Hit const& hit)
			{
				if (hit.state != hit.ENTER)
					return;

				if (auto pIngr{ hit.object.HasComponent<IngredientEnity>() })
				{
					if (pIngr->IsFalling() && anima.Refs().get<components::ObjectTransform>()->position.y < pIngr->Refs().get<components::ObjectTransform>()->position.y)
						anima.DoDeath();
				}
				else if (hit.object.HasComponent<int/*attack cloud*/>())
				{
					anima.DoHit();
				}
			}
		);
	};

	m_Handles = std::make_shared<decltype(makeHandle())>(makeHandle());
}

void EnemyController::OnUpdate()
{
}

void EnemyController::OnDisable()
{
	m_Handles = {};
}
