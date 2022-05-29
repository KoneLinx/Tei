#include "Entity.h"

void Entity::SetStateHandler(std::function<void(Entity&)> handler)
{
	m_StateHandler = std::move(handler);
}

void Entity::Update()
{
	if (m_StateHandler)
		m_StateHandler(*this);
}

void EntityController::HandleUpdate(Entity& entity)
{
	auto& [transform] = entity.refs;

	if (m_Controller)
	{
		auto input = m_Controller();
		transform.get().position += input * tei::Time->thread->delta.count();
		// check bounds
	}

}

EntityPlayerController::EntityPlayerController(int id)
	: id{ id }
{
	
	auto setAction = [this] (Action action)
	{
		return [=, this] (bool state) 
		{ 
			if (state)
				m_Action = action;
			else if (m_Action == action)
				m_Action = {};
		};
	};

	// Controller
	{
		m_Input.AddCommand(tei::input::ControllerInput::Button::DPAD_UP   .WithIndex(id), setAction(Action::UP));
		m_Input.AddCommand(tei::input::ControllerInput::Button::DPAD_DOWN .WithIndex(id), setAction(Action::DOWN));
		m_Input.AddCommand(tei::input::ControllerInput::Button::DPAD_LEFT .WithIndex(id), setAction(Action::LEFT));
		m_Input.AddCommand(tei::input::ControllerInput::Button::DPAD_RIGHT.WithIndex(id), setAction(Action::RIGHT));
		m_Input.AddCommand(tei::input::ControllerInput::Button::X         .WithIndex(id), setAction(Action::ATTACK));
	}

	// Keyboard
	if (id == 0)
	{
		m_Input.AddCommand(tei::input::KeyboardInput::Main::Z, setAction(Action::UP));
		m_Input.AddCommand(tei::input::KeyboardInput::Main::S, setAction(Action::DOWN));
		m_Input.AddCommand(tei::input::KeyboardInput::Main::A, setAction(Action::LEFT));
		m_Input.AddCommand(tei::input::KeyboardInput::Main::D, setAction(Action::RIGHT));
		m_Input.AddCommand(tei::input::KeyboardInput::Main::SPACE, setAction(Action::ATTACK));
	}
	else if (id == 1)
	{
		m_Input.AddCommand(tei::input::KeyboardInput::Arrow::UP, setAction(Action::UP));
		m_Input.AddCommand(tei::input::KeyboardInput::Arrow::DOWN, setAction(Action::DOWN));
		m_Input.AddCommand(tei::input::KeyboardInput::Arrow::LEFT, setAction(Action::LEFT));
		m_Input.AddCommand(tei::input::KeyboardInput::Arrow::RIGHT, setAction(Action::RIGHT));
		m_Input.AddCommand(tei::input::KeyboardInput::Mod::RCTRL, setAction(Action::ATTACK));
	}
	else std::cerr << "[Warning] player with (id > 1) has no keyboard bindings" << std::endl;
}

tei::unit::Vec2 EntityPlayerController::QueryInput()
{

	switch (m_Action)
	{
	case Action::NONE:
		return {  0,  0 };
		break;
	case Action::LEFT:
		return { -1,  0 };
		break;
	case Action::RIGHT:
		return {  1,  0 };
		break;
	case Action::UP:
		return {  0,  1 };
		break;
	case Action::DOWN:
		return {  0, -1 };
		break;
	}

	return {};

}

void burger::entity::Entity::LoadFromObject(nlohmann::json const& data, Object& parent)
{
	auto& entityObject = parent.AddChild();

	auto src{ LoadOptional<std::string_view>(data, "sprite/src") };
	if (!src) 
		src = "default_sprite.png";

	entityObject.AddComponent(Resources->LoadShared<resource::Sprite>(*src, 1_s, 1, 1));

}

