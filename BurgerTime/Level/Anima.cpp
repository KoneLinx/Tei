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
		Hitbox{},
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
			else
			{
				if (m_Movement[dim] != 0 && std::signbit(m_Movement[dim]) == std::signbit(factor))
					m_Movement[dim] = {};
			}
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
							m_AllowX = std::max(0, m_AllowX + (tei::unit::Unit(bool(hit.state)) ? 1 : -1));

						else if (pEntity->Type() == StaticEntityData::LADDER)
							m_AllowY = std::max(0, m_AllowY + (tei::unit::Unit(bool(hit.state)) ? 1 : -1));
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
				input::KeyboardInput::Arrow::RIGHT.WithState(input::BinaryState::ANY),
				updateMovement(1.f, 0)
			),
			Input->AddCommand(
				input::KeyboardInput::Arrow::LEFT.WithState(input::BinaryState::ANY),
				updateMovement(-1.f, 0)
			),
			Input->AddCommand(
				input::KeyboardInput::Arrow::UP.WithState(input::BinaryState::ANY),
				updateMovement(1.f, 1)
			),
			Input->AddCommand(
				input::KeyboardInput::Arrow::DOWN.WithState(input::BinaryState::ANY),
				updateMovement(-1.f, 1)
			),
			Input->AddCommand(
				input::ControllerInput::Stick::LEFT.WithState({ { .25f, .25f }, { 1.f, 1.f } }),
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
