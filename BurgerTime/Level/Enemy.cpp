#include "pch.h"
#include "Enemy.h"
#include "Player.h"
#include "Hitbox.h"
#include "Ingredient.h"
#include "Score.h"
#include "Level.h"
#include <numeric>

using namespace tei::components;

EnemyController::EnemyController(AnimaData const& data)
	: m_pData{ &data }
	, m_OldMovement{}
{}

void EnemyController::OnEnable(tei::ecs::Object& object)
{
	auto parent = &object.GetParent();
	while (!parent->HasComponent<Level>())
		parent = &parent->GetParent();

	m_Level = &parent->GetComponent<Level>();
}

void EnemyController::OnUpdate()
{
	auto& [anima, transform] = Refs();

	auto newMovement = anima.GetAllowedMovement();

	if (   m_OldMovement.left > 0  != newMovement.left > 0
		|| m_OldMovement.right > 0 != newMovement.right > 0
		|| m_OldMovement.up > 0    != newMovement.up > 0
		|| m_OldMovement.down > 0  != newMovement.down > 0)
	{
		m_OldMovement = newMovement;
		
		auto movement = anima.GetMovement();

		newMovement.right &= movement.x >= 0 ? ~0 : 0;
		newMovement.left  &= movement.x <= 0 ? ~0 : 0;
		newMovement.up    &= movement.y >= 0 ? ~0 : 0;
		newMovement.down  &= movement.y <= 0 ? ~0 : 0;

		if (   newMovement.left  == 0
			&& newMovement.right == 0
			&& newMovement.up    == 0
			&& newMovement.down  == 0)
		{
			if (newMovement.up > 0)
				anima.SetInput({ 0,  1 });
			else if (newMovement.down > 0)
				anima.SetInput({ 0, -1 });
			else if (newMovement.left > 0)
				anima.SetInput({ -1, 0 });
			else if (newMovement.right > 0)
				anima.SetInput({  1, 0 });
			else
				anima.SetInput({ 0, 0 });

			return;
		}

		unit::Vec2 pos{ transform->position };

		auto distance = [pos] (Anima const& player) -> unit::Vec2
		{
			return player.Refs().get<ObjectTransform>()->position - pos;
		};
		auto distanceF = [&distance] (Anima const& player) -> float
		{
			return float(distance(player).length());
		};

		pos = distance(*std::ranges::min_element(
			m_Level->GetPlayers(),
			std::less{},
			distanceF
		));

		pos.y = std::abs(pos.x) > std::abs(pos.y * 2) ? 0 : pos.y;

		movement.y = pos.y < 0 ? -float(newMovement.down > 0) : float(newMovement.up    > 0);
		movement.x = pos.x < 0 ? -float(newMovement.left > 0) : float(newMovement.right > 0);
		movement.x = movement.y == 0 ? movement.x : 0;

		if (dot(movement, movement) == 1)
			return anima.SetInput(movement);
		
		movement.x = /*movement.y != 0 ? 0.f :*/
			  bool(newMovement.right) ?  1.f
			: bool(newMovement.left ) ? -1.f
			: 0.f;
		movement.y = movement.x != 0 ? 0.f :
			  bool(newMovement.up   ) ?  1.f
			: bool(newMovement.down ) ? -1.f
			: 0.f;

		anima.SetInput(movement);
	}
}

void EnemyController::OnDisable()
{}

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
				
				else if (hit.object.HasComponent<Cloud>())
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
