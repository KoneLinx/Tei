#pragma once

#include <variant>
#include <tei/utility.h>

namespace tei::internal::input
{
	
	enum struct DeviceId : uint16_t {
		KEYBOARD,
		MOUSE,
		CONTROLER,
	};

	template <typename Data_t, typename State_t>
	struct InputType
	{
		using Data = Data_t;
		using State = State_t;

		DeviceId deviceId{};
		uint16_t deviceIndex{};
		uint32_t keyId{};

		State_t state{};

		bool onChange{ true };

		constexpr InputType() = default;
		constexpr InputType(DeviceId deviceId, uint32_t keyId, bool onChange = true, State state = {}, uint16_t deviceIndex = {});

		constexpr InputType WithChange(bool onChange) const;
		constexpr InputType WithState(State state) const;
		constexpr InputType WithIndex(uint16_t deviceIndex) const;

		constexpr bool operator == (InputType const& other) const;
	};
	
	enum struct BinaryState
	{
		PRESSED,
		RELEASED,
		ANY,
	};

	using BinaryData = bool;
	using AnalogData = float;
	using AnalogState = std::pair<AnalogData, AnalogData>;
	using Analog2Data = std::pair<AnalogData, AnalogData>;
	using Analog2State = std::pair<Analog2Data, Analog2Data>;

	using InputBinary = InputType<bool, BinaryState>;
	using InputAnalog = InputType<AnalogData, AnalogState>;
	using InputAnalog2 = InputType<Analog2Data, Analog2State>;

	constexpr bool operator & (InputBinary const& input, BinaryData const& value);
	constexpr bool operator & (InputAnalog const& input, AnalogData const& value);
	constexpr bool operator & (InputAnalog2 const& input, Analog2Data const& value);

	using SomeCommonInputType = std::variant<InputBinary, InputAnalog, InputAnalog2>;
	using SomeCommonInputData = std::variant<InputBinary::Data, InputAnalog::Data, InputAnalog2::Data>;

	using SomeCommonInputTypeRef = utility::ApplyTrait_t<std::reference_wrapper, utility::ApplyTrait_t<std::add_const_t, SomeCommonInputType>>;
	using SomeCommonInputDataRef = utility::ApplyTrait_t<std::reference_wrapper, utility::ApplyTrait_t<std::add_const_t, SomeCommonInputData>>;

	constexpr bool operator & (InputBinary const& input, BinaryData const& value)
	{
		return
			input.state == BinaryState::ANY ||
			input.state == (value ? BinaryState::PRESSED : BinaryState::RELEASED);
	}

	constexpr bool operator & (InputAnalog const& input, AnalogData const& value)
	{
		auto [low, high] = input.state;
		return
			low <= value &&
			value <= high;
	}
	
	constexpr bool operator & (InputAnalog2 const& input, Analog2Data const& value)
	{
		auto [low1, high1] = input.state.first;
		auto [low2, high2] = input.state.second;
		return
			low1 <= value.first &&
			value.first <= high1 &&
			low2 <= value.second &&
			value.second <= high2;
	}

	template <typename InputType, typename Data = typename InputType::Data>
	constexpr bool operator & (Data const& value, InputType const& input)
	{
		return input & value;
	}

	template<typename Data_t, typename State_t>
	inline constexpr InputType<Data_t, State_t>::InputType(DeviceId deviceId, uint32_t keyId, bool onChange, State_t state, uint16_t deviceIndex)
		: deviceId{ deviceId }
		, keyId{ keyId }
		, deviceIndex{ deviceIndex }
		, onChange{ onChange }
		, state{ state }
	{}

	template<typename Data_t, typename State_t>
	inline constexpr InputType<Data_t, State_t> InputType<Data_t, State_t>::WithChange(bool ponChange) const
	{
		auto out = *this;
		out.onChange = ponChange;
		return out;
	}

	template<typename Data_t, typename State_t>
	inline constexpr InputType<Data_t, State_t> InputType<Data_t, State_t>::WithState(State pstate) const
	{
		auto out = *this;
		out.state = pstate;
		return out;
	}

	template<typename Data_t, typename State_t>
	inline constexpr InputType<Data_t, State_t> InputType<Data_t, State_t>::WithIndex(uint16_t pdeviceIndex) const
	{
		auto out = *this;
		out.deviceIndex = pdeviceIndex;
		return out;
	}

	template<typename Data_t, typename State_t>
	inline constexpr bool InputType<Data_t, State_t>::operator==(InputType const& other) const
	{
		return
			deviceId == other.deviceId &&
			deviceIndex == other.deviceIndex &&
			keyId == other.keyId;
	}

}

namespace tei::external::input
{
	using DeviceId = tei::internal::input::DeviceId;
	using tei::internal::input::InputBinary;
	using tei::internal::input::BinaryState;
	using tei::internal::input::InputAnalog;
	using tei::internal::input::AnalogState;
	using tei::internal::input::InputAnalog2;
	using tei::internal::input::Analog2State;
}