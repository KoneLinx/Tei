#pragma once

#include <variant>

namespace tei::internal::input
{
	
	enum struct DeviceId : uint16_t {
		KEYBOARD,
		MOUSE,
		CONTROLER,
	};

	template <typename Data_t>
	struct InputType
	{
		using Data = Data_t;

		DeviceId deviceId;
		uint32_t keyId;
		uint16_t deviceIndex;

		constexpr InputType(DeviceId deviceId, uint32_t keyId, uint16_t deviceIndex = {}) noexcept;
		constexpr InputType(InputType const&, uint16_t deviceIndex) noexcept;
	};

	using InputBinary = InputType<bool>;
	using InputAnalog = InputType<float>;
	using InputAnalog2 = InputType<std::pair<float, float>>;

	using SomeCommonInputType = std::variant<InputBinary, InputAnalog, InputAnalog2>;
	using SomeCommonInputData = std::variant<InputBinary::Data, InputAnalog::Data, InputAnalog2::Data>;
	
	template<typename Data_t>
	inline constexpr InputType<Data_t>::InputType(DeviceId deviceId, uint32_t keyId, uint16_t deviceIndex) noexcept
		: deviceId{ deviceId }
		, keyId{ keyId }
		, deviceIndex{ deviceIndex }
	{}

	template<typename Data_t>
	inline constexpr InputType<Data_t>::InputType(InputType const& toCopy, uint16_t deviceIndex) noexcept
		: InputType{ toCopy.deviceId, toCopy.keyId, deviceIndex }
	{}

}

namespace tei::external::input
{
	using DeviceId = tei::internal::input::DeviceId;
	using tei::internal::input::InputBinary;
	using tei::internal::input::InputAnalog;
	using tei::internal::input::InputAnalog2;
}