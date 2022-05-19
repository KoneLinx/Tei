#include "pch.h"
#include "Player.h"
#include "Hitbox.h"
#include "Enemy.h"

#include <array>
#include <tuple>
#include <memory>
#include <utility>

using namespace input;

PlayerController::PlayerController(AnimaData const& data)
	: m_playerID{ data.id }
{}

void PlayerController::OnEnable(tei::ecs::Object& object)
{
	auto& anima = object.GetComponent<Anima>();

	auto updateMovementKeyboard = [this, &anima] (float factor, int dim)
	{
		return [=, this, &anima] (input::BinaryData input)
		{
			if (input)
			{
				m_Movement[dim] = float(input) * factor;
				anima.SetInput(m_Movement);
			}
			else if (m_Movement[dim] != 0 && std::signbit(m_Movement[dim]) == std::signbit(factor))
			{
				m_Movement[dim] = {};
				anima.SetInput(m_Movement);
			}
		};
	};

	auto updateMovementController = [this, &anima] (input::Analog2Data input)
	{
		m_Movement = { input.first, input.second };
		anima.SetInput(m_Movement);
	};

	auto doAttack = [&] { anima.DoAttack(); };

	auto onHit = [&] (Hitbox::Hit const& hit)
	{
		if (hit.state != hit.ENTER)
			return;

		if (auto pEnemy{ hit.object.HasComponent<EnemyController>() })
		{
			if (pEnemy->Refs().get<Anima>().IsActive())
				anima.DoDeath();
		}
	};
	auto& hitbox = object.GetComponent<Hitbox>();

	auto makeHandles = [&]
	{
		int id = m_playerID;
		auto const movestate = BinaryState::ANY;
		auto const actstate = BinaryState::PRESSED;
		auto const stickstate = Analog2State{ { .25f, .25f }, { 1.f, 1.f } };
		return std::tuple
		{
			Input->AddCommand( std::array{ KeyboardInput::Main::W.WithState(movestate)                    , KeyboardInput::Arrow::UP   .WithState(movestate)                }[id], updateMovementKeyboard( 1.f, 1) ),
			Input->AddCommand( std::array{ KeyboardInput::Main::A.WithState(movestate)                    , KeyboardInput::Arrow::LEFT .WithState(movestate)                }[id], updateMovementKeyboard(-1.f, 0) ),
			Input->AddCommand( std::array{ KeyboardInput::Main::S.WithState(movestate)                    , KeyboardInput::Arrow::DOWN .WithState(movestate)                }[id], updateMovementKeyboard(-1.f, 1) ),
			Input->AddCommand( std::array{ KeyboardInput::Main::D.WithState(movestate)                    , KeyboardInput::Arrow::RIGHT.WithState(movestate)                }[id], updateMovementKeyboard( 1.f, 0) ),
			Input->AddCommand( std::array{ KeyboardInput::Main::F.WithState(actstate)                     , KeyboardInput::Mod::LCTRL  .WithState(actstate)                 }[id], doAttack                        ),
			Input->AddCommand( std::array{ ControllerInput::Stick::LEFT.WithIndex(0).WithState(stickstate), ControllerInput::Stick::LEFT.WithIndex(1).WithState(stickstate) }[id], updateMovementController        ),
			Input->AddCommand( std::array{ ControllerInput::Button::X.WithIndex(0).WithState(actstate)    , ControllerInput::Button::X.WithIndex(1).WithState(actstate)     }[id], doAttack                        ),
			hitbox.AddObserver(onHit)
		};
	};

	// Because std::any can only hold copyable types, there seem to be few options other than to wrap it in a shared_ptr
	// And since we need the type before we know the value, it is wrapped in a lambda, then used in a decltype.

	m_Handles = std::make_shared<decltype(makeHandles())>(makeHandles());
}

void PlayerController::OnDisable()
{
	m_Handles = {};
}
