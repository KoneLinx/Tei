#pragma once

#include <type_traits>
#include <concepts>
#include <functional>
#include <variant>
#include <any>
#include <tei/internal/utility.h>
#include "InputType.h"

namespace tei::internal::input
{

	template <typename InputType_t, typename Data = typename InputType_t::Data>
	class Command
	{
	public:

		using InputType = InputType_t;

		template <std::invocable Action>
		Command(InputType input, Action action);

		template <std::invocable<Data const&> Action>
		Command(InputType input, Action action);
		
		template <typename UserData, std::invocable<Data const&, UserData&> Action>
		Command(InputType input, Action action, UserData&& userdata);
		
		InputType const& GetInputType() const noexcept;
		void SetInputType(InputType) noexcept;

		bool HasUserData() const noexcept; 
		template <typename UserData>
		UserData& GetUserData() const;
		template <typename UserData>
		void SetUserData(UserData&& data);

		void Execute(Data const&) const;

	private:

		InputType m_InputType;
		std::function<void(Data const&)> const m_Action;
		std::any m_Data;
	};

	template <typename Input, typename ... Arg>
	Command(Input, Arg...) -> Command<Input, typename Input::Data>;

	using CommandBinary = Command<InputBinary>;
	using CommandAnalog = Command<InputAnalog>;
	using CommandAnalog2 = Command<InputAnalog2>;

	template <typename InputType, typename Data>
	template <std::invocable Action>
	inline Command<InputType, Data>::Command(InputType input, Action action)
		: Command{
			input,
			[action = std::move(action), this] (Data const&)
			{
				std::invoke(action);
			}
		}
	{}

	template <typename InputType, typename Data>
	template <std::invocable<Data const&> Action>
	inline Command<InputType, Data>::Command(InputType input, Action action)
		: m_InputType{ input }
		, m_Data{}
		, m_Action{ std::move(action) }
	{}

	template<typename InputType, typename Data>
	template<typename UserData, std::invocable<Data const&, UserData&> Action>
	inline Command<InputType, Data>::Command(InputType input, Action action, UserData&& data)
		: Command{
			input, 
			[action = std::move(action), this] (InputType const& input)
			{
				std::invoke(action, input, std::any_cast<UserData&>(this->m_Data));
			}
		}
	{}

	template<typename InputType, typename Data>
	inline InputType const& Command<InputType, Data>::GetInputType() const noexcept
	{
		return m_InputType;
	}

	template<typename InputType, typename Data>
	inline void Command<InputType, Data>::SetInputType(InputType input) noexcept
	{
		m_InputType = input;
	}

	template<typename InputType, typename Data>
	inline bool Command<InputType, Data>::HasUserData() const noexcept
	{
		return m_Data.has_value();
	}

	template<typename InputType, typename Data>
	inline void Command<InputType, Data>::Execute(Data const& data) const
	{
		std::invoke(m_Action, data);
	}

	template<typename InputType, typename Data>
	template <typename UserData>
	inline UserData& Command<InputType, Data>::GetUserData() const
	{
		return std::any_cast<UserData&>(m_Data);
	}

	template<typename InputType, typename Data>
	template<typename UserData>
	inline void Command<InputType, Data>::SetUserData(UserData&& data)
	{
		if (!HasUserData())
			throw utility::TeiRuntimeError{ "Command has no user data" };
		else
			m_Data = std::forward<UserData>(data);
	}

}

namespace tei::external::input
{
	using tei::internal::input::Command;
	using tei::internal::input::CommandBinary;
	using tei::internal::input::CommandAnalog;
	using tei::internal::input::CommandAnalog2;
}