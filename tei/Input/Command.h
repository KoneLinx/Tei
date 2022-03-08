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

	template <typename InputType, typename Data = typename InputType::Data>
	class Command
	{
	public:

		template <std::invocable<Data const&> Action>
		Command(InputType input, Action action, bool onChangeOnly = true);
		
		template <typename UserData, std::invocable<Data const&, UserData&> Action>
		Command(InputType input, Action action, UserData&& userdata, bool onChangeOnly = true);
		
		InputType GetInputType() const noexcept;
		void SetInputType(InputType) noexcept;
		
		bool OnChangeOnly() const noexcept;

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
		bool m_ChangeOnly;
	};

	using CommandBinary = Command<InputBinary>;
	using CommandAnalog = Command<InputAnalog>;
	using CommandAnalog2 = Command<InputAnalog2>;


	template <typename InputType, typename Data>
	template <std::invocable<Data const&> Action>
	inline Command<InputType, Data>::Command(InputType input, Action action, bool onChangeOnly)
		: m_InputType{ input }
		, m_Data{}
		, m_Action{ std::move(action) }
		, m_ChangeOnly{ onChangeOnly }
	{}

	template<typename InputType, typename Data>
	template<typename UserData, std::invocable<Data const&, UserData&> Action>
	inline Command<InputType, Data>::Command(InputType input, Action action, UserData&& data, bool onChangeOnly)
		: Command{
			input, 
			[action = std::move(action), this] (InputType const& input)
			{
				action(input, std::any_cast<UserData&>(this->m_Data));
			}, 
			onChangeOnly
		}
	{}

	template<typename InputType, typename Data>
	inline InputType Command<InputType, Data>::GetInputType() const noexcept
	{
		return m_InputType;
	}

	template<typename InputType, typename Data>
	inline void Command<InputType, Data>::SetInputType(InputType input) noexcept
	{
		m_InputType = input;
	}

	template<typename InputType, typename Data>
	inline bool Command<InputType, Data>::OnChangeOnly() const noexcept
	{
		return m_ChangeOnly;
	}

	template<typename InputType, typename Data>
	inline bool Command<InputType, Data>::HasUserData() const noexcept
	{
		return m_Data.has_value();
	}

	template<typename InputType, typename Data>
	inline void Command<InputType, Data>::Execute(Data const& data) const
	{
		m_Action(data);
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
}