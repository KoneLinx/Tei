#include "pch.h"
#include "Menu.h"
#include "../Level/Level.h"
#include "GameManager.h"

using namespace tei;
using namespace components;
using namespace unit;
using namespace std::literals;

Menu::Menu(std::vector<std::string_view> textFields)
	: m_Updater{}
	, m_Storage{ std::move(textFields) }
{}

void Menu::OnInitialize(tei::ecs::Object& object)
{
	auto& textFields = std::any_cast<std::vector<std::string_view> const&>(m_Storage);

	auto& listObject = object.AddChild();

	Position const beginPos{ 0, textFields.size() / 2.f };
	Position const incrementPos{ 0, -2 };

	auto addButton = [&, pos = beginPos, item = int()] (std::string_view text) mutable
	{
		auto & child = listObject.AddChild();
		auto& transform = child.AddComponent(
			ObjectTransform{ pos }
		);

		auto& visual = child.AddChild();
		visual.AddComponents(
			Observed<std::string>{ std::string{ text } },
			Resources->LoadShared<resource::Font>("resources/FalconPatrol.ttf", 72),
			TextRenderComponent{}
		);
		auto& colour = visual.AddComponent(
			Observed<unit::Colour>{ { 1, 1, 1, 1 } }
		);

		pos += Vec2{ incrementPos };
		return [&, thisItem = item++] (int item)
		{
			if (item == thisItem)
			{
				colour = { 1, 1, 0, 1 };
				transform.get().scale = Scale{ 1.1f };
			}
			else
			{
				colour = { 1, 1, 1, 1 };
				transform.get().scale = Scale{ 1.f };
			}
		};
	};

	std::vector<std::function<void(int)>> updaters(textFields.size());

	std::ranges::transform(
		textFields,
		updaters.begin(),
		addButton
	);

	auto clickSound = Resources->LoadShared<resource::Sound>("resources/click.wav");

	m_Updater = [&, clickSound, updaters = std::move(updaters), index = int()] (int change) mutable
	{
		Audio->Play(clickSound);
		if (change == 0)
		{
			object.SetState(false);
			return this->Notify(std::exchange(index, {}));
		}
		else
		{
			index = std::clamp<int>(index + change, 0, int(updaters.size() - 1));
			for (auto& act : updaters)
				act(index);
		}
	};

	m_Storage = {};
}

void Menu::OnEnable()
{
	using namespace input;

	auto makeHandles = [this]
	{
		return std::tuple{
			Input->AddCommand(
				KeyboardInput::Arrow::DOWN,
				[this] { m_Updater(1); }
			),
			Input->AddCommand(
				KeyboardInput::Arrow::UP,
				[this] { m_Updater(-1); }
			),
			Input->AddCommand(
				KeyboardInput::Main::S,
				[this] { m_Updater(1); }
			),
			Input->AddCommand(
				KeyboardInput::Main::W,
				[this] { m_Updater(-1); }
			),
			Input->AddCommand(
				KeyboardInput::Main::SPACE,
				[this] { m_Updater(0); }
			),
			Input->AddCommand(
				KeyboardInput::Main::F,
				[this] { m_Updater(0); }
			),
			Input->AddCommand(
				KeyboardInput::Main::RETURN,
				[this] { m_Updater(0); }
			),
			Input->AddCommand(
				ControllerInput::Button::DPAD_DOWN,
				[this] { m_Updater(1); }
			),
			Input->AddCommand(
				ControllerInput::Button::DPAD_UP,
				[this] { m_Updater(-1); }
			),
			Input->AddCommand(
				ControllerInput::Button::X,
				[this] { m_Updater(0); }
			)
		};
	};

	m_Updater(-1);

	m_Storage = std::make_shared<decltype(makeHandles())>(makeHandles());
}

void Menu::OnDisable()
{
	m_Storage = {};
}

tei::ecs::Object& CreateMainMenu(tei::ecs::Object& parent)
{
	auto& menuObject = parent.AddChild();

	// Title
	{
		menuObject.AddChild()
		.AddComponents(
			ObjectTransform{ { Position{ 0, .30f }, Scale{ 0.05f } } },
			Observed<std::string>{ "BURGERTIME"s },
			Observed<Colour>{ { 1, .33f, .33f, 1 } },
			Resources->LoadShared<resource::Font>("resources/FalconPatrol.ttf", 72),
			TextRenderComponent{}
		);
	}

	// Menu
	{
		auto& listObject = menuObject.AddChild();
		listObject.AddComponent(
			ObjectTransform{ { Position{ 0, -.125f }, Scale{ 0.05f } } }
		);
		listObject.AddComponent(
			Menu{ {
				"Single player",
				"Co op",
				"Versus mode",
				"Quit"
			} }
		).AddObserver(
			[&](Menu::Event index)
			{
				if (index >= 3)
					return Application->Quit();
				menuObject.SetState(false);
				auto& game = CreateGame(parent, std::array{ "single"sv, "co-op"sv, "versus"sv } [index] );
				auto start = Time->frame.now;
				game.GetComponent<Level>().AddObserver(
					[&](Level::LevelEvent const& event)
					{
						if (event.type == event.ALL_COMPLETED || event.type == event.FAILED)
						{
							game.SetState(false);
							CreateScoreMenu(parent, game, Time->frame.now - start, event.type != event.ALL_COMPLETED);
						}
					}
				).Detach();
				game.AddComponent(
					tei::Input->AddCommand(
						input::KeyboardInput::Main::ESCAPE,
						[](bool)
						{
							tei::Event->Notify(GameReset{});
						}
					)
				);
			}
		).Detach();
	}

	menuObject.AddComponent(
		tei::Input->AddCommand(
			input::KeyboardInput::Main::ESCAPE,
			[&] (bool)
			{
				if (menuObject.IsActive())
					tei::Application->Quit();
			}
		)
	);

	return menuObject;
}

tei::ecs::Object& CreateScoreMenu(tei::ecs::Object& parent, tei::ecs::Object& level, Clock::duration durtation, bool failed)
{
	auto& board = parent.AddChild();

	auto font = Resources->LoadShared<resource::Font>("resources/FalconPatrol.ttf", 72);

	auto& list = board.AddChild();
	
	auto& listTranform = list.AddComponent(ObjectTransform{ unit::Scale{ 0.05f } });
	
	auto addText = [&, pos = unit::Position{ 0, .35f / listTranform->scale.y }](/*tei::ecs::Object& object,*/ std::string text, unit::Colour colour = { 1,1,1,1 }, unit::Scale scale = { 1,1 }) mutable -> tei::ecs::Object&
	{
		auto& child = list.AddChild();

		child.AddComponent(ObjectTransform{ { pos, scale } });
		pos.y -= scale.y * 2.5f;

		auto& visual = child.AddChild();
		visual.AddComponents(
			Observed{ std::move(text) },
			Observed{ colour },
			TextRenderComponent{},
			font
		);

		return child;
	};

	auto score = level.GetComponent<ScoreManager>().GetScore();

	addText(failed ? "Failed" : "You won", { failed ? 1 : 0, failed ? 0 : 1, 0, 1 }, unit::Scale{ 2 });
	addText("score", { .75, .75, .75, 1 });
	addText(std::to_string(score));
	addText("time", { .75, .75, .75, 1 });
	addText(std::to_string(std::chrono::duration_cast<std::chrono::seconds>(durtation).count()));

	board.AddComponent(
		Input->AddCommand(
			input::KeyboardInput::Main::RETURN,
			[](bool)
			{
				Event->Notify(GameReset{});
			}
		)
	);

	return board;
}
