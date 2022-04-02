#pragma once

#include <type_traits>
#include <concepts>
#include <functional>
#include <variant>
#include <any>
#include <tei/internal/Utility/Error.h>
#include "InputType.h"

namespace tei::internal::input
{

	template <typename InputType_t = void, typename Data = typename InputType_t::Data>
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
		
		InputType const& GetInput() const noexcept;
		void SetInput(InputType) noexcept;

		bool HasData() const noexcept; 
		template <typename UserData>
		UserData& GetData() const;
		template <typename UserData>
		void SetData(UserData&& data);

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
	inline InputType const& Command<InputType, Data>::GetInput() const noexcept
	{
		return m_InputType;
	}

	template<typename InputType, typename Data>
	inline void Command<InputType, Data>::SetInput(InputType input) noexcept
	{
		m_InputType = input;
	}

	template<typename InputType, typename Data>
	inline bool Command<InputType, Data>::HasData() const noexcept
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
	inline UserData& Command<InputType, Data>::GetData() const
	{
		return std::any_cast<UserData&>(m_Data);
	}

	template<typename InputType, typename Data>
	template<typename UserData>
	inline void Command<InputType, Data>::SetData(UserData&& data)
	{
		if (!HasData())
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