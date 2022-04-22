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

//void EntityPlayerController::AddTo(EntityController & controller, int id)
//{
//	controller.SetInputController(
//		[self = EntityPlayerController{ id }] () mutable 
//		{ 
//			return self.QueryInput();
//		}
//	);
//}

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

burger::entity::Hitbox::Hitbox(std::shared_ptr<std::vector<Hitbox*>> others)
	: m_pParent{ nullptr }
	, m_Objects { std::move(others) }
	, m_Hit{ false }
{}

void burger::entity::Hitbox::OnEnbale(Object& parent)
{
	m_pParent = &parent;
	m_Objects->push_back(this);
	m_Hit.checkout();
}


void burger::entity::Hitbox::OnDisable()
{
	m_Objects->erase(std::ranges::find(*m_Objects, this));
	m_Hit.checkout();
}

void burger::entity::Hitbox::OnUpdate()
{
	if (std::ranges::subrange matches{ std::ranges::find(*m_Objects, this), std::ranges::end(*m_Objects) }; matches.size() > 1)
	{
		for (auto match : matches)
		{
			if (CollidesWith(*match))
			{
				m_Hit = match;
				return;
			}
		}
	}
}

bool IsOverlapping(tei::unit::Transform const& a, tei::unit::Transform const& b)
{
#if false /*circle*/
	auto min = (a.scale.x + a.scale.y + b.scale.x + b.scale.y) / 2;
	auto diff = b.position - a.position;
	return min * min > glm::dot(diff, diff);
#else /*rectangle*/
	auto min = a.scale + b.scale / 2.f;
	auto diff{ glm::abs(a.position - b.position) };
	return min.x > diff.x && min.t > diff.y;
#endif
}

bool burger::entity::Hitbox::CollidesWith(Hitbox const& other) const
{
	auto& [selfTransform] = refs;
	auto& [otherTransform] = other.refs;
	return IsOverlapping(selfTransform, otherTransform);
}

std::optional<burger::entity::Object&> burger::entity::Hitbox::HitObject() const
{
	if (m_Hit.check())
		return *(*m_Hit)->m_pParent;
	else
		return std::nullopt;
}
