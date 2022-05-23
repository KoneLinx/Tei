#include "pch.h"
#include "Anima.h"

#include "Hitbox.h"
#include "StaticEntity.h"
#include "Ingredient.h"
#include "Player.h"
#include "Particle.h"
#include "Enemy.h"

#include <memory>
#include <algorithm>

using namespace tei;
using namespace components;

using enum AnimaData::State::ID;

tei::ecs::Object& Anima::Create(tei::ecs::Object& parent, AnimaData const& data, tei::unit::Position spawnPoint)
{
	auto& object = parent.AddChild();
	auto& self = object.AddComponent<Anima>();

	self.m_pData = &data;
	self.m_SpawnPoint = spawnPoint;

	object.AddComponents(
		ObjectTransform{ spawnPoint },
		data.box,
		Hitbox{},
		data.state.sprites.front(),
		SpriteRenderComponent{}
	);

	if (data.type == data.PLAYER)
		object.AddComponent<PlayerController>(data);
	else
		object.AddComponent<EnemyController>(data);
	
	if (data.hostile)
		object.AddComponent<EnemyEffects>();
	else
		object.AddComponent<PlayerEffects>();
	
	auto addZone = [&](unit::Position offset, auto& var, auto type) -> auto&
	{
		auto& child = object.AddChild();
		child.AddComponents(
			ObjectTransform{ offset },
			Box{ .1f, .1f }
		);
		child.AddComponent<Hitbox>().AddObserver(
			[&var, type] (Hitbox::Hit const& hit)
			{
				if (auto pEntity{ hit.object.HasComponent<StaticEntity>() })
				{
					if (pEntity->Type() == type)
						var += bool(hit.state) ? 1 : -1;
				}
			}
		).Detach();
		return child;
	};

	[[maybe_unused]] auto& leftZone  = addZone({ -.6f, -.1f }, self.m_AllowL, StaticEntityData::PLATFORM);
	[[maybe_unused]] auto& rightZone = addZone({  .6f, -.1f }, self.m_AllowR, StaticEntityData::PLATFORM);
	[[maybe_unused]] auto& downZone  = addZone({    0, -.6f }, self.m_AllowD, StaticEntityData::LADDER);
	[[maybe_unused]] auto& upZone    = addZone({    0, -.4f }, self.m_AllowU, StaticEntityData::LADDER);

	return object;
}

bool any(auto const& var, auto const& ... val)
{
	return ((var == val) || ...);
}

void Anima::OnEnable(tei::ecs::Object const& /*object*/)
{
	//auto& hitbox = object.GetComponent<Hitbox>();

	//auto makeHandles = [&]
	//{
	//	return hitbox.AddObserver(
	//		[this] (Hitbox::Hit const& hit)
	//		{
	//			if (auto pEntity{ hit.object.HasComponent<StaticEntity>() })
	//			{
	//				if (pEntity->Type() == StaticEntityData::PLATFORM || pEntity->Type() == StaticEntityData::SHELF)
	//					m_AllowX = std::max(0, m_AllowX + (tei::unit::Unit(bool(hit.state)) ? 1 : -1));

	//				else if (pEntity->Type() == StaticEntityData::LADDER)
	//					m_AllowY = std::max(0, m_AllowY + (tei::unit::Unit(bool(hit.state)) ? 1 : -1));
	//			}
	//		}
	//	);
	//};

	// Because std::any can only hold copyable types, there seem to be few options other than to wrap it in a shared_ptr
	// And since we need the type before we know the value, it is wrapped in a lambda, then used in a decltype.

	//m_Handles = std::make_shared<decltype(makeHandles())>(makeHandles());

}

void Anima::OnUpdate()
{
	auto& [transform, sprite] = Refs();

	if (!any(m_State, HIT, DYING, ATTACKING))
	{
		unit::Vec2 movement{
			std::clamp<float>(m_Movement.x, -float(m_AllowL > 0), float(m_AllowR > 0)),
			std::clamp<float>(m_Movement.y, -float(m_AllowD > 0), float(m_AllowU > 0))
		};

		movement *= Time->frame.delta.count() * 2.5f;

		if (dot(movement, movement) > 0)
		{
			transform.get().position += movement;

			if (std::abs(movement.y) > 0)
				m_State = movement.y > 0 
					? WALKING_UP
					: WALKING_DOWN;
			else
				m_State = movement.x > 0 
					? WALKING_RIGHT 
					: WALKING_LEFT;
		}
		else
		{
			m_State = STATIONARY;
		}
	}
	else if (m_Timer)
	{
		if (std::exchange(m_State, STATIONARY) == DYING)
		{
			transform.get().position = m_SpawnPoint;
		}
	}

	sprite = m_pData->state.sprites[size_t(m_State)];

}

void Anima::OnDisable()
{
	m_Handles = {};
}

void Anima::DoAttack()
{
	if (!any(m_State, HIT, DYING))
	{
		m_State = ATTACKING;
		m_pData->state.sprites[size_t(ATTACKING)]->origintime = Time->frame.now;
		m_Timer = .5_s;
	}
}

void Anima::DoDeath()
{
	if (m_State != DYING)
	{
		m_State = DYING;
		m_pData->state.sprites[size_t(DYING)]->origintime = Time->frame.now;
		m_Timer = 2_s;
	}
}

void Anima::DoHit()
{
	if (m_State != DYING)
	{
		m_State = HIT;
		m_Timer = 2_s;
	}
}

bool Anima::IsActive()
{
	return !any(m_State, DYING, HIT);
}

bool Anima::IsAlive()
{
	return m_State != DYING;
}

