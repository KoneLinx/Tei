#include "pch.h"
#include "Anima.h"

#include "Hitbox.h"
#include "StaticEntity.h"
#include "Ingredient.h"
#include "Player.h"
#include "Enemy.h"

#include <memory>

using namespace tei;
using namespace components;

using enum AnimaData::State::ID;

tei::ecs::Object& Anima::Create(tei::ecs::Object& object, AnimaData const& data)
{
	auto& self = object.AddComponent<Anima>();

	self.m_pData = &data;

	object.AddComponents(
		data.box,
		Hitbox{},
		data.state.sprites.front(),
		SpriteRenderComponent{}
	);

	if (data.type == data.PLAYER)
		object.AddComponent<PlayerController>(data);
	else
		object.AddComponent<EnemyController>(data);

	return object;
}

bool any(auto const& var, auto const& ... val)
{
	return ((var == val) || ...);
}

void Anima::OnEnable(tei::ecs::Object const& object)
{
	auto& hitbox = object.GetComponent<Hitbox>();

	auto makeHandles = [&]
	{
		return hitbox.AddObserver(
			[this](Hitbox::Hit const& hit)
			{
				if (auto pEntity{ hit.object.HasComponent<StaticEntity>() })
				{
					if (pEntity->Type() == StaticEntityData::PLATFORM || pEntity->Type() == StaticEntityData::SHELF)
						m_AllowX = std::max(0, m_AllowX + (tei::unit::Unit(bool(hit.state)) ? 1 : -1));

					else if (pEntity->Type() == StaticEntityData::LADDER)
						m_AllowY = std::max(0, m_AllowY + (tei::unit::Unit(bool(hit.state)) ? 1 : -1));
				}
			}
		);
	};

	// Because std::any can only hold copyable types, there seem to be few options other than to wrap it in a shared_ptr
	// And since we need the type before we know the value, it is wrapped in a lambda, then used in a decltype.

	m_Handles = std::make_shared<decltype(makeHandles())>(makeHandles());

}

void Anima::OnUpdate()
{
	auto& [transform, sprite] = Refs();

	if (!any(m_State, HIT, DYING, ATTACKING))
	{
		unit::Vec2 velocity{ (m_AllowX ? 1.f : 0.001f) * m_Movement.x, (m_AllowY ? 1.f : 0.001f) * m_Movement.y };
		velocity *= Time->frame.delta.count() * 2.f;

		if (dot(velocity, velocity) > 0)
		{
			transform.get().position += velocity;

			if (std::abs(velocity.y) > 0)
				m_State = velocity.y > 0 
					? WALKING_UP
					: WALKING_DOWN;
			else
				m_State = velocity.x > 0 
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
		m_State = STATIONARY;
	}

	sprite = m_pData->state.sprites[size_t(m_State)];

}

void Anima::OnDisable()
{
	m_Handles = {};
}

void Anima::DoAttack()
{
	m_State = ATTACKING;
	m_pData->state.sprites[size_t(ATTACKING)]->origintime = Time->frame.now;
	m_Timer = .5_s;
}

void Anima::DoDeath()
{
	m_State = DYING;
	m_pData->state.sprites[size_t(DYING)]->origintime = Time->frame.now;
	m_Timer = 3_s;
}

void Anima::DoHit()
{
	m_State = HIT;
	m_Timer = 3_s;
}

bool Anima::IsActive()
{
	return !any(m_State, DYING, HIT);
}

