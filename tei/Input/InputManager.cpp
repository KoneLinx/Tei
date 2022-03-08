#include "InputManager.h"

#include <optional>
#include <limits>
#include <ranges>

#include "Controller.h"

#pragma warning(disable: 5105) // macro error in windows.h
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <Xinput.h>
#pragma comment(lib, "xinput.lib")

using namespace tei::internal::input;

InputManager::Service Input{};

struct InputManager::PollData
{
	XINPUT_STATE currentState, previousState;
};

template <typename InputType>
auto TestInput(InputManager::PollData const&, InputType, bool);

bool TestInput(XINPUT_STATE const& state, InputBinary input)
{
	return (state.Gamepad.wButtons & input.keyId) != 0;
}

template <>
auto TestInput<InputBinary>(InputManager::PollData const& data, InputBinary input, bool change)
{
	using Ret = std::optional<bool>;

	if (input.deviceId == DeviceId::CONTROLER)
	{
		if (auto state = TestInput(data.currentState, input); !change || state != TestInput(data.previousState, input))
			return Ret{ bool{ state } };
		else 
			return Ret{};
	}
	
	return Ret{};
}

template <typename Decimal, typename Value>
Decimal OverMax(Value val) noexcept
{
	return static_cast<Decimal>(val) / static_cast<Decimal>(std::numeric_limits<Value>::max());
}

template <>
auto TestInput<InputAnalog>(InputManager::PollData const& data, InputAnalog input, bool change)
{
	using Ret = std::optional<InputAnalog::Data>;

	if (input.deviceId == DeviceId::CONTROLER)
	{
		switch (input.keyId)
		{
		case ControllerInput::Trigger::Index::LEFT:
			if (!change || data.currentState.Gamepad.bLeftTrigger != data.previousState.Gamepad.bLeftTrigger)
				return Ret{ OverMax<InputAnalog::Data>(data.currentState.Gamepad.bLeftTrigger) };
		case ControllerInput::Trigger::Index::RIGHT:
			if (!change || data.currentState.Gamepad.bRightTrigger != data.previousState.Gamepad.bRightTrigger)
				return Ret{ OverMax<InputAnalog::Data>(data.currentState.Gamepad.bRightTrigger) };
		}
		return Ret{};
	}

	return Ret{};
}

template <>
auto TestInput<InputAnalog2>(InputManager::PollData const& data, InputAnalog2 input, bool change)
{
	using Data_t = InputAnalog2::Data::first_type;
	using Ret = std::optional<InputAnalog2::Data>;

	if (input.deviceId == DeviceId::CONTROLER)
	{
		switch (input.keyId)
		{
		case ControllerInput::Stick::Index::LEFT:
			if (!change || data.currentState.Gamepad.sThumbLX != data.previousState.Gamepad.sThumbLX || data.currentState.Gamepad.sThumbLY != data.previousState.Gamepad.sThumbLY)
			return Ret{{
				OverMax<Data_t>(data.currentState.Gamepad.sThumbLX),
				OverMax<Data_t>(data.currentState.Gamepad.sThumbLY)
			}};
		case ControllerInput::Stick::Index::RIGHT:
			if (!change || data.currentState.Gamepad.sThumbRX != data.previousState.Gamepad.sThumbRX || data.currentState.Gamepad.sThumbRY != data.previousState.Gamepad.sThumbRY)
			return Ret{{
				OverMax<Data_t>(data.currentState.Gamepad.sThumbRX),
				OverMax<Data_t>(data.currentState.Gamepad.sThumbRY)
			}};
		}
		return Ret{};
	}

	return Ret{};
}

template <typename InputType>
void Update(InputManager::PollData const& data, std::vector<std::unique_ptr<Command<InputType>>> const& commands)
{
	for (auto& command : commands)
		if (auto result{ TestInput(data, command->GetInputType(), command->OnChangeOnly()) })
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

	data.previousState = std::exchange(data.currentState, {});
	XInputGetState(0, &data.currentState);

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
	return TestInput(*m_PollData, button, false).value_or(false);
}

SomeCommonInputData tei::internal::input::InputManager::GetInputDataImpl(SomeCommonInputType input) const
{
	auto& data = *m_PollData;
	return std::visit(
		[&] <typename InputType> (InputType input) -> SomeCommonInputData
		{
			return TestInput(data, input, false).value_or(typename InputType::Data{});
		},
		input
	);
}
