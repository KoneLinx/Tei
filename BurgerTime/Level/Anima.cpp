#include "pch.h"
#include "Anima.h"

#include "Hitbox.h"
#include "StaticEntity.h"
#include "Ingredient.h"

#include <memory>

using namespace tei;
using namespace components;

tei::ecs::Object& Anima::Create(tei::ecs::Object& object, AnimaData const& data)
{
	auto& self = object.AddComponent<Anima>();

	self.m_pData = &data;

	object.AddComponents(
		data.box,
		Hitbox{ {} },
		data.state.sprites.front(),
		SpriteRenderComponent{}
	);

	return object;
}

void Anima::OnEnable(tei::ecs::Object const& object)
{
	auto& hitbox = object.GetComponent<Hitbox>();

	auto updateMovement = [this](float factor, int dim)
	{
		return [=, this](input::BinaryData input)
		{
			if (input)
				m_Movement[dim] = float(input) * factor;
			else if (m_Movement[dim] != 0 && std::signbit(m_Movement[dim]) == std::signbit(factor))
				m_Movement.x = {};
		};
	};

	auto makeHandles = [&]
	{
		return std::tuple
		{
			hitbox.AddObserver(
				[this](Hitbox::Hit const& hit)
				{
					if (auto pEntity{ hit.object.HasComponent<StaticEntity>() })
					{
						if (pEntity->Type() == StaticEntityData::PLATFORM || pEntity->Type() == StaticEntityData::SHELF)
							m_AllowedMovement.x = tei::unit::Unit(bool(hit.state));

						else if (pEntity->Type() == StaticEntityData::LADDER)
							m_AllowedMovement.y = tei::unit::Unit(bool(hit.state));
					}

					if (hit.state != hit.ENTER)
						return;

					if (auto pEnemy{ hit.object.HasComponent<Anima>() })
					{
						if (pEnemy->m_pData->type != m_pData->type || /*TODO level == versus && self == player*/false)
						{
							if (m_pData->type == AnimaData::PLAYER)
							{
								m_State = AnimaData::State::DYING;
								m_pData->state.sprites[size_t(AnimaData::State::DYING)]->origintime = Time->frame.now;
							}
							else
							{
								m_State = AnimaData::State::HIT;
							}
							m_Timer = 3_s;
						}
					}

					if (auto pIngr{ hit.object.HasComponent<IngredientEnity>() })
					{
						if (/*TODO falling*/false)
						{
							puts("Hit by ingredient");
							m_State = AnimaData::State::DYING;
							m_Timer = 3_s;
						}
					}
				}
			),
			Input->AddCommand(
				input::KeyboardInput::Arrow::RIGHT,
				updateMovement(1, 0)
			),
			Input->AddCommand(
				input::KeyboardInput::Arrow::LEFT,
				updateMovement(-1, 0)
			),
			Input->AddCommand(
				input::KeyboardInput::Arrow::UP,
				updateMovement(1, 1)
			),
			Input->AddCommand(
				input::KeyboardInput::Arrow::DOWN,
				updateMovement(-1, 1)
			),
			Input->AddCommand(
				input::ControllerInput::Stick::LEFT.WithState({ { .25, .25 }, { 1, 1 } }),
				[this] (input::Analog2Data input)
				{
					m_Movement = { input.first, input.second };
				}
			)
		};
	};

	// Because std::any can only hold copyable types, there seem to be few options other than to wrap it in a shared_ptr
	// And since we need the type before we know the value, it is wrapped in a lambda, then used in a decltype.

	m_Storage = std::make_shared<decltype(makeHandles())>(makeHandles());

}

void Anima::OnUpdate()
{
	auto& [transform, sprite] = Refs();

	using enum AnimaData::State::ID;

	auto oldState = m_State;

	if (m_State != DYING || m_State != HIT)
	{
		unit::Vec2 velocity{ m_Movement * m_AllowedMovement * Time->frame.delta.count() };

		if (dot(velocity, velocity) > 0.1)
		{
			if (std::abs(velocity.y) > 0.1)
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
			//if attacking
			//m_State = AnimaData::State::ATTACKING;
		}
		//if dying or hit
		//m_State = AnimaData::State::HIT;
		//m_State = AnimaData::State::DYING;
	}

	if (m_State == HIT && m_Timer)
		m_State = STATIONARY;

	sprite = m_pData->state.sprites[size_t(m_State)];

}

void Anima::OnDisable()
{
	m_Storage = {};
}
