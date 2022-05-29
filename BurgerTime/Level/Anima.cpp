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

	self.m_pParent = &parent;
	self.m_pObject = &object;
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
			Box{ .2f, .2f }
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

	[[maybe_unused]] auto& leftZone  = addZone({ -.4f, -.1f }, self.m_AllowedMovement.left , StaticEntityData::PLATFORM);
	[[maybe_unused]] auto& rightZone = addZone({  .4f, -.1f }, self.m_AllowedMovement.right, StaticEntityData::PLATFORM);
	[[maybe_unused]] auto& downZone  = addZone({    0, -.6f }, self.m_AllowedMovement.down , StaticEntityData::LADDER);
	[[maybe_unused]] auto& upZone    = addZone({    0, -.4f }, self.m_AllowedMovement.up   , StaticEntityData::LADDER);

	return object;
}

bool any(auto const& var, auto const& ... val)
{
	return ((var == val) || ...);
}

void Anima::OnEnable(tei::ecs::Object& /*object*/)
{}

void Anima::OnUpdate()
{
	auto& [transform, sprite] = Refs();

	if (!any(m_State, HIT, DYING, ATTACKING))
	{
		if (m_AllowedMovement.down == 0 && m_AllowedMovement.up == 0 && m_AllowedMovement.left == 0 && m_AllowedMovement.right == 0)
			DoDeath();

		unit::Vec2 movement{
			std::clamp<float>(m_Movement.x, -float(m_AllowedMovement.left > 0), float(m_AllowedMovement.right > 0)),
			std::clamp<float>(m_Movement.y, -float(m_AllowedMovement.down > 0), float(m_AllowedMovement.up > 0))
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
		m_pData->state.sprites[size_t(ATTACK_PARTICLE)]->origintime = Time->frame.now;
		m_Timer = .5_s;

		Particle::Create(
			*m_pObject, 
			unit::Position{ m_Movement / 2.f },
			unit::Position{ m_Movement / 1.5f },
			m_pData->state.sprites[size_t(ATTACK_PARTICLE)],
			.5_s
		).AddComponents(
			Cloud{},
			Box{ 1, 1 },
			Hitbox{}
		);
		
		{
			static auto s = Resources->LoadShared<resource::Sound>("resources/attack.wav");
			Audio->Play(s);
		}
	}
}

void Anima::DoDeath(bool stayDead)
{
	if (m_State != DYING)
	{
		m_State = DYING;
		m_pData->state.sprites[size_t(DYING)]->origintime = Time->frame.now;
		if (stayDead)
			m_Timer = Clock::time_point::max();
		else 
			m_Timer = 2_s;
	

		if (!m_pData->hostile)
		{
			static auto s = Resources->LoadShared<resource::Sound>("resources/death.wav");
			Audio->Play(s);
		}
		else
		{
			static auto s = Resources->LoadShared<resource::Sound>("resources/crush.wav");
			Audio->Play(s);
		}
	}
}

void Anima::DoHit()
{
	if (m_State != DYING)
	{
		m_State = HIT;
		m_Timer = 2_s;

		{
			static auto s = Resources->LoadShared<resource::Sound>("resources/hit.wav");
			Audio->Play(s);
		}
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

