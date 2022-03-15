#include "InputManager.h"

#include <optional>
#include <limits>
#include <ranges>
#include <array>

#include "Controller.h"

#include <SDL.h>

#pragma warning(disable: 5105) // macro error in windows.h
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <Xinput.h>
#pragma comment(lib, "xinput.lib")

tei::internal::input::InputManager::Service tei::internal::input::Input{};

using namespace tei::internal::input;

struct Controller
{
	XINPUT_STATE currentState{}, previousState{};
};

struct Keyboard
{
	using State = std::array<uint8_t, SDL_NUM_SCANCODES>;
	State state[2]{};
	State* currentState{ state }, * previousState{ state + 1 };
};

struct InputManager::PollData
{
	Keyboard keyboard;
	Controller controller;
};

template <typename InputType>
auto TestInput(InputManager::PollData const&, InputType);

bool TestInput(XINPUT_STATE const& state, InputBinary const& input)
{
	return (state.Gamepad.wButtons & input.keyId) != 0;
}

bool TestInput(Keyboard::State const& state, InputBinary const& input)
{
	return state[input.keyId] != 0;
}

template <>
auto TestInput<InputBinary>(InputManager::PollData const& data, InputBinary input)
{
	using Ret = std::optional<bool>;

	bool value{};
	bool change{ !input.onChange };

	switch (input.deviceId)
	{
	case DeviceId::KEYBOARD:
	{
		change = change || value != TestInput(*data.keyboard.previousState, input);
		if (change)
			value  = TestInput(*data.keyboard.currentState, input);
	}
	break;
	case DeviceId::CONTROLER:
	{
		change = change || value != TestInput(data.controller.previousState, input);
		if (change)
			value  = TestInput(data.controller.currentState, input);
	}
	break;
	}
	
	bool state{
		input.state == BinaryState::ANY ||
		input.state == (value ? BinaryState::PRESSED : BinaryState::RELEASED)
	};
	
	if (change && state)
		return Ret{ bool(value) };
	else
		return Ret{};
}

template <typename Decimal, typename Value>
Decimal OverMax(Value val) noexcept
{
	return static_cast<Decimal>(val) / static_cast<Decimal>(std::numeric_limits<Value>::max());
}

template <>
auto TestInput<InputAnalog>(InputManager::PollData const& data, InputAnalog input)
{
	using Ret = std::optional<InputAnalog::Data>;

	InputAnalog::Data value{};
	bool change{ !input.onChange };

	if (input.deviceId == DeviceId::CONTROLER)
	{
		switch (input.keyId)
		{
		case ControllerInput::Trigger::Index::LEFT:
		{
			change = change || data.controller.currentState.Gamepad.bLeftTrigger != data.controller.previousState.Gamepad.bLeftTrigger;
			if (change)
				value = OverMax<InputAnalog::Data>(data.controller.currentState.Gamepad.bLeftTrigger);
		}
		break;
		case ControllerInput::Trigger::Index::RIGHT:
		{
			change = change || data.controller.currentState.Gamepad.bRightTrigger != data.controller.previousState.Gamepad.bRightTrigger;
			if (change)
				value = OverMax<InputAnalog::Data>(data.controller.currentState.Gamepad.bLeftTrigger);
		}
		break;
		}
	}

	auto [low, high] = input.state;
	bool state{
		low <= value &&
		value <= high
	};

	if (change && state)
		return Ret{ std::move(value) };
	else
		return Ret{};
}

template <>
auto TestInput<InputAnalog2>(InputManager::PollData const& data, InputAnalog2 input)
{
	using Data_t = InputAnalog2::Data::first_type;
	using Ret = std::optional<InputAnalog2::Data>;

	InputAnalog2::Data value{};
	bool change{ !input.onChange };

	if (input.deviceId == DeviceId::CONTROLER)
	{
		switch (input.keyId)
		{
		case ControllerInput::Stick::Index::LEFT:
		{
			change = change
				|| data.controller.currentState.Gamepad.sThumbLX != data.controller.previousState.Gamepad.sThumbLX 
				|| data.controller.currentState.Gamepad.sThumbLY != data.controller.previousState.Gamepad.sThumbLY;
			if (change)
				value = {
					OverMax<Data_t>(data.controller.currentState.Gamepad.sThumbLX),
					OverMax<Data_t>(data.controller.currentState.Gamepad.sThumbLY)
				};
		}
		break;
		case ControllerInput::Stick::Index::RIGHT:
		{
			change = change
				|| data.controller.currentState.Gamepad.sThumbRX != data.controller.previousState.Gamepad.sThumbRX 
				|| data.controller.currentState.Gamepad.sThumbRY != data.controller.previousState.Gamepad.sThumbRY;
			if (change)
				value = {
					OverMax<Data_t>(data.controller.currentState.Gamepad.sThumbRX),
					OverMax<Data_t>(data.controller.currentState.Gamepad.sThumbRY)
				};
		}
		break;
		}
	}

	auto [low1, high1] = input.state.first;
	auto [low2, high2] = input.state.second;
	bool state{
		low1 <= value.first &&
		value.first <= high1 &&
		low2 <= value.second &&
		value.second <= high2
	};

	if (change && state)
		return Ret{ std::move(value) };
	else
		return Ret{};
}

template <typename InputType>
void Update(InputManager::PollData const& data, std::vector<std::unique_ptr<Command<InputType>>> const& commands)
{
	for (auto& command : commands)
		if (auto result{ TestInput(data, command->GetInputType()) })
			command->Execute(*result);
}

InputManager::InputManager()
	: m_PollData{ std::make_unique<PollData>() }
	, m_Commands{}
{}

// in .cpp: important! PollData destructor
InputManager::~InputManager()
{}

void InputManager::ProcessInput()
{
	InputManager::PollData& data{ *m_PollData };

	int stateSize{};
	uint8_t const* stateData{ SDL_GetKeyboardState(&stateSize) };
	std::swap(data.keyboard.previousState, data.keyboard.currentState);
	std::ranges::copy_n(stateData, stateSize, data.keyboard.currentState->data());
	std::ranges::fill_n(data.keyboard.currentState->data() + stateSize, data.keyboard.currentState->size() - stateSize, uint8_t{});

	data.controller.previousState = std::exchange(data.controller.currentState, {});
	XInputGetState(0, &data.controller.currentState);

	std::apply(
		[&] (auto& ... lists) 
		{ 
			(Update(data, lists), ...);
		}, 
		m_Commands
	);
}

bool InputManager::IsPressed(InputBinary button) const
{
	return TestInput(*m_PollData, button).value_or(false);
}

SomeCommonInputData tei::internal::input::InputManager::GetInputDataImpl(SomeCommonInputType input) const
{
	auto& data = *m_PollData;
	return std::visit(
		[&] <typename InputType> (InputType input) -> SomeCommonInputData
		{
			return TestInput(data, input).value_or(typename InputType::Data{});
		},
		input
	);
}
