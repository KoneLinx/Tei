#include "pch.h"
#include "Player.h"
#include "Hitbox.h"
#include "Enemy.h"
#include "Level.h"

#include <array>
#include <tuple>
#include <memory>
#include <utility>

using namespace input;

PlayerController::PlayerController(AnimaData const& data)
	: m_playerID{ data.id }
{}

auto& GetLevel(tei::ecs::Object const& object)
{
	auto* pParent = &object;
	Level* pLevel{};
	while ((pLevel = pParent->HasComponent<Level>()) == nullptr)
		pParent = &pParent->GetParent();
	return *pLevel;
};

void PlayerController::OnEnable(tei::ecs::Object const& object)
{
	auto& level = GetLevel(object);

	auto& [anima] = Refs();

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

	auto doAttack = [&]
	{
		if (object.HasComponent<PlayerEffects>() != nullptr && level.DoPlayerAttack())
			anima.DoAttack(); 
	};

	auto makeHandles = [&]
	{
		int id = m_playerID;
		auto const movestate = BinaryState::ANY;
		auto const actstate = BinaryState::PRESSED;
		auto const stickstate = Analog2State{ { .05f, 1.f }, { .05f, 1.f } };
		auto controllerWalk = ControllerInput::Stick::LEFT.WithState(stickstate);
		auto controllerAttack = ControllerInput::Button::A.WithState(actstate);

		return std::tuple
		{
			Input->AddCommand( std::array{ KeyboardInput::Main::W.WithState(movestate)   , KeyboardInput::Arrow::UP   .WithState(movestate) }[id], updateMovementKeyboard( 1.f, 1) ),
			Input->AddCommand( std::array{ KeyboardInput::Main::A.WithState(movestate)   , KeyboardInput::Arrow::LEFT .WithState(movestate) }[id], updateMovementKeyboard(-1.f, 0) ),
			Input->AddCommand( std::array{ KeyboardInput::Main::S.WithState(movestate)   , KeyboardInput::Arrow::DOWN .WithState(movestate) }[id], updateMovementKeyboard(-1.f, 1) ),
			Input->AddCommand( std::array{ KeyboardInput::Main::D.WithState(movestate)   , KeyboardInput::Arrow::RIGHT.WithState(movestate) }[id], updateMovementKeyboard( 1.f, 0) ),
			Input->AddCommand( std::array{ KeyboardInput::Main::SPACE.WithState(actstate), KeyboardInput::Mod::RCTRL  .WithState(actstate)  }[id], doAttack                        ),
			Input->AddCommand( std::array{ controllerWalk.WithIndex(0)                   , controllerWalk.WithIndex(1)                      }[id], updateMovementController),
			Input->AddCommand( std::array{ controllerAttack.WithIndex(0)                 , controllerAttack.WithIndex(1)                    }[id], doAttack                        ),
			
			Input->AddCommand( std::array{ ControllerInput::Button::DPAD_UP.WithIndex(0).WithState(movestate)   , ControllerInput::Button::DPAD_UP.WithIndex(1).WithState(movestate)    }[id], updateMovementKeyboard( 1.f, 1) ),
			Input->AddCommand( std::array{ ControllerInput::Button::DPAD_LEFT.WithIndex(0).WithState(movestate) , ControllerInput::Button::DPAD_LEFT.WithIndex(1).WithState(movestate)  }[id], updateMovementKeyboard(-1.f, 0) ),
			Input->AddCommand( std::array{ ControllerInput::Button::DPAD_DOWN.WithIndex(0).WithState(movestate) , ControllerInput::Button::DPAD_DOWN.WithIndex(1).WithState(movestate)  }[id], updateMovementKeyboard(-1.f, 1) ),
			Input->AddCommand( std::array{ ControllerInput::Button::DPAD_RIGHT.WithIndex(0).WithState(movestate), ControllerInput::Button::DPAD_RIGHT.WithIndex(1).WithState(movestate) }[id], updateMovementKeyboard( 1.f, 0) ),
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

void PlayerEffects::OnEnable(tei::ecs::Object& object)
{
	auto& level = GetLevel(object);

	auto makeHandles = [&]
	{
		return object.GetComponent<Hitbox>().AddObserver(
			[&](Hitbox::Hit const& hit)
			{
				if (hit.state != hit.ENTER)
					return;

				if (auto pEnemy{ hit.object.HasComponent<EnemyEffects>() })
				{
					if (pEnemy->Refs().get<Anima>().IsActive())
					{
						Refs().get<Anima>().DoDeath(!level.DoPlayerDeath());
					}
				}
			}
		);
	};
	m_Handles = std::make_shared<decltype(makeHandles())>(makeHandles());
}

void PlayerEffects::OnDisable()
{
	m_Handles = {};
}

tei::ecs::Object& PlayerAttackCount::Create(tei::ecs::Object& parent)
{
	auto& object = parent.AddChild();
	auto [self, text] = object.AddComponents(
		PlayerAttackCount{},
		tei::components::Observed<std::string>{}
	);
	object.AddComponents(
		tei::components::TextRenderComponent{},
		Resources->LoadShared<tei::resource::Font>("resources/FalconPatrol.ttf", 26)
	);

	parent.GetComponent<Level>().AddObserver(
		[&] (Level::LevelDataEvent const& event)
		{
			if (event.data.index() == event.ATTACKCOUNT)
			{
				text = std::to_string(std::get<event.ATTACKCOUNT>(event.data));
			}
		}
	).Detach();

	return object;
}

