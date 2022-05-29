#include "teipch.h"
#include "InputManager.h"

#include <algorithm>
#include <optional>
#include <variant>
#include <utility>
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
	std::array<Controller, 4> controllers;
};

bool TestInput(XINPUT_STATE const& state, InputBinary const& input)
{
	return (state.Gamepad.wButtons & input.keyId) != 0;
}

bool TestInput(Keyboard::State const& state, InputBinary const& input)
{
	return state[input.keyId] != 0;
}

auto TestInput(InputManager::PollData const& data, InputBinary const& input)
{
	using Ret = std::optional<InputBinary::Data>;

	InputBinary::Data value{};
	bool change{ !input.onChange };

	switch (input.deviceId)
	{
	case DeviceId::KEYBOARD:
	{
		value  = TestInput(*data.keyboard.currentState, input);
		change = change || value != TestInput(*data.keyboard.previousState, input);
	}
	break;
	case DeviceId::CONTROLER:
	{
		value  = TestInput(data.controllers[input.deviceIndex].currentState, input);
		change = change || value != TestInput(data.controllers[input.deviceIndex].previousState, input);
	}
	break;
	}
	
	if (change && input & value)
		return Ret{ value };
	else 
		return Ret{};
}

template <typename Decimal, typename Value>
Decimal OverMax(Value val) noexcept
{
	return static_cast<Decimal>(val) / (static_cast<Decimal>(std::numeric_limits<Value>::max()) + 1);
}

auto TestInput(InputManager::PollData const& data, InputAnalog const& input)
{
	using Ret = std::optional<InputAnalog::Data>;

	InputAnalog::Data value{};
	bool change{ !input.onChange };

	if (input.deviceId == DeviceId::CONTROLER)
	{
		auto const& state = data.controllers[input.deviceIndex];

		switch (input.keyId)
		{
		case ControllerInput::Trigger::Index::LEFT:
		{
			value = {
				OverMax<InputAnalog::Data>(state.currentState.Gamepad.bLeftTrigger)
			};
			if (!change)
			{
				InputAnalog::Data prev{
					OverMax<InputAnalog::Data>(state.previousState.Gamepad.bLeftTrigger)
				};
				change = (input & value) != (input & prev);
			}
			change = change || input & value && (
				   state.currentState.Gamepad.bLeftTrigger != state.previousState.Gamepad.bLeftTrigger
			);
		}
		break;
		case ControllerInput::Trigger::Index::RIGHT:
		{
			value = {
				OverMax<InputAnalog::Data>(state.currentState.Gamepad.bRightTrigger)
			};
			if (!change)
			{
				InputAnalog::Data prev{
					OverMax<InputAnalog::Data>(state.previousState.Gamepad.bRightTrigger)
				};
				change = (input & value) != (input & prev);
			}
			change = change || input & value && (
				   state.currentState.Gamepad.bRightTrigger != state.previousState.Gamepad.bRightTrigger
			);
		}
		break;
		}
	}
	
	if (change)
	{
		if (input& value)
			return Ret{ std::move(value) };
		else
			return Ret{ InputAnalog::Data{} };
	}
	else return Ret{};
}

auto TestInput(InputManager::PollData const& data, InputAnalog2 const& input)
{
	using Data_t = InputAnalog2::Data::first_type;
	using Ret = std::optional<InputAnalog2::Data>;

	InputAnalog2::Data value{};
	bool change{ !input.onChange };

	if (input.deviceId == DeviceId::CONTROLER)
	{
		auto const& state = data.controllers[input.deviceIndex];

		switch (input.keyId)
		{
		case ControllerInput::Stick::Index::LEFT:
		{
			value = {
				OverMax<Data_t>(state.currentState.Gamepad.sThumbLX),
				OverMax<Data_t>(state.currentState.Gamepad.sThumbLY)
			};
			if (!change)
			{
				InputAnalog2::Data prev{
					OverMax<Data_t>(state.previousState.Gamepad.sThumbLX),
					OverMax<Data_t>(state.previousState.Gamepad.sThumbLY)
				};
				change = (input & value) != (input & prev);
			}
			change = change || input & value && (
				   state.currentState.Gamepad.sThumbLX != state.previousState.Gamepad.sThumbLX 
				|| state.currentState.Gamepad.sThumbLY != state.previousState.Gamepad.sThumbLY
			);
		}
		break;
		case ControllerInput::Stick::Index::RIGHT:
		{
			value = {
				OverMax<Data_t>(state.currentState.Gamepad.sThumbRX),
				OverMax<Data_t>(state.currentState.Gamepad.sThumbRY)
			};
			if (!change)
			{
				InputAnalog2::Data prev{
					OverMax<Data_t>(state.previousState.Gamepad.sThumbRX),
					OverMax<Data_t>(state.previousState.Gamepad.sThumbRY)
				};
				change = (input & value) != (input & prev);
			}
			change = change || input & value && (
				   state.currentState.Gamepad.sThumbRX != state.previousState.Gamepad.sThumbRX 
				|| state.currentState.Gamepad.sThumbRY != state.previousState.Gamepad.sThumbRY
			);
		}
		break;
		}
	}
	
	if (change)
	{
		if (input& value)
			return Ret{ std::move(value) };
		else
			return Ret{ InputAnalog2::Data{} };
	}
	else return Ret{};
}

template <typename InputType>
void Update(InputManager::PollData const& data, auto const& commands)
{
	for (auto& [type, any] : commands)
	{
		auto& command = std::any_cast<Command<InputType>&>(any);
		if (auto result{ TestInput(data, command.GetInput()) })
			command.Execute(*result);
	}
}

InputManager::InputManager()
	: m_PollData{ std::make_unique<PollData>() }
	, m_Commands{ std::make_shared<CommandContainer>() }
{}

// in .cpp: important! PollData destructor
InputManager::~InputManager()
{}

void InputManager::RemoveCommand(CommandHandle& handle)
{
	handle = {};
}

void InputManager::ProcessInput()
{
	InputManager::PollData& data{ *m_PollData };

	int stateSize{};
	uint8_t const* stateData{ SDL_GetKeyboardState(&stateSize) };
	std::swap(data.keyboard.previousState, data.keyboard.currentState);
	std::ranges::copy_n(stateData, stateSize, data.keyboard.currentState->data());
	std::ranges::fill_n(data.keyboard.currentState->data() + stateSize, data.keyboard.currentState->size() - stateSize, uint8_t{});

	for (int index{}; auto & state : data.controllers)
	{
		state.previousState = std::exchange(state.currentState, {});
		XInputGetState(index++, &state.currentState);
	}

	auto updater = [&] <typename InputType> ()
	{
		auto subrange = utility::SubrangeFromPair(m_Commands->equal_range(typeid(InputType)));
		Update<InputType>(data, subrange);
	};

	updater.operator()<InputBinary>();
	updater.operator()<InputAnalog>();
	updater.operator()<InputAnalog2>();

}

bool InputManager::IsPressed(InputBinary const& button) const
{
	return TestInput(*m_PollData, button).value_or(false);
}

InputManager::CommandHandle InputManager::AddCommand(CommandContainer::value_type value)
{
	return { m_Commands, m_Commands->insert(std::move(value)) };
}

SomeCommonInputData tei::internal::input::InputManager::GetInputImpl(SomeCommonInputTypeRef input) const
{
	auto& data = *m_PollData;
	return std::visit(
		[&] <typename InputType> (std::reference_wrapper<InputType> input) -> SomeCommonInputData
		{
			return TestInput(data, input.get()).value_or(typename InputType::Data{});
		},
		input
	);
}

void InputManager::InvokeInputImpl(SomeCommonInputTypeRef input, SomeCommonInputDataRef dataref) const
{
	std::visit(
		[this, &dataref] <typename InputType> (std::reference_wrapper<InputType const> invoked)
		{
			auto& data = std::get<std::reference_wrapper<typename InputType::Data const>>(dataref).get();
			for (auto& [type, any] : utility::SubrangeFromPair(m_Commands->equal_range(typeid(InputType))))
			{
				auto& command = std::any_cast<Command<InputType> const&>(any);
				if (auto& input{ command.GetInput() }; invoked.get() == input && input & data)
					command.Execute(data);
			}
		},
		input
	);
}

InputManager::CommandHandle::~CommandHandle()
{
	if (auto ptr{ m_Container.lock() })
		ptr->erase(m_Position);
}

InputManager::CommandHandle& InputManager::CommandHandle::operator=(CommandHandle&& other)
{
	if (auto ptr{ std::exchange(m_Container, std::move(other.m_Container)).lock() })
		ptr->erase(m_Position);
	m_Position = std::move(other.m_Position);
	return *this;
}

bool InputManager::CommandHandle::Alive()
{
	return !m_Container.expired();
}

void InputManager::CommandHandle::Clear()
{
	*this = {};
}

void InputManager::CommandHandle::Detach()
{
	m_Container = {};
	m_Position = {};
}


InputManager::CommandHandle::CommandHandle(std::weak_ptr<CommandContainer> container, CommandContainer::iterator at)
	: m_Container{ container }
	, m_Position{ at }
{}