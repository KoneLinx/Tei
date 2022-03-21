
#pragma once

#include <vector>
#include <memory>
#include <tuple>
#include <typeindex>

#include "InputType.h"
#include "Command.h"

#include <tei/internal/utility.h>

namespace tei::internal::input
{

	class InputManager
	{
	public:

		InputManager();
		~InputManager();

		template <typename InputType>
		Command<InputType>& AddCommand(Command<InputType> command = {});
		
		template <typename InputType, typename Action> requires std::constructible_from<Command<InputType>, InputType, Action>
		Command<InputType>& AddCommand(InputType input, Action action);

		template <typename InputType>
		void RemoveCommand(Command<InputType> const&);

		void RemoveCommand(utility::AnyRef);

		void ProcessInput();

		bool IsPressed(InputBinary const& button) const;

		template <typename InputType>
		typename InputType::Data GetInputData(InputType input) const;

		template <typename InputType, typename Data = typename InputType::Data>
		void InvokeInput(InputType input, Data state);

	private:

		SomeCommonInputData GetInputImpl(SomeCommonInputTypeRef) const;
		void InvokeInputImpl(SomeCommonInputTypeRef, SomeCommonInputDataRef) const;

		std::unordered_multimap<std::type_index, std::any> m_Commands;
		std::unordered_multimap<utility::AnyRef, decltype(m_Commands)::const_iterator> m_CommandByData;

		struct PollData;
		std::unique_ptr<PollData> m_PollData;

	public:

		using Service = utility::Service<InputManager>;

	};

	extern InputManager::Service Input;

	template <typename InputType>
	inline Command<InputType>& InputManager::AddCommand(Command<InputType> command)
	{
		auto it = m_Commands.insert({
			typeid(InputType),
			std::move(command)
		});
		auto& data = std::any_cast<Command<InputType>&>(it->second);
		m_CommandByData.insert({
			data,
			it
		});
		return data;
	}

	template <typename InputType, typename Action> requires std::constructible_from<Command<InputType>, InputType, Action>
	inline Command<InputType>& InputManager::AddCommand(InputType input, Action action)
	{
		return AddCommand(Command<InputType>{ input, std::move(action) });
	}

	template<typename InputType>
	inline void InputManager::RemoveCommand(Command<InputType> const& toRemove)
	{
		RemoveCommand(utility::AnyRef{ toRemove });
	}

	template<typename InputType>
	inline typename InputType::Data InputManager::GetInputData(InputType input) const
	{
		return std::get<typename InputType::Data>(GetInputImpl(std::move(input)));
	}

	template<typename InputType, typename Data>
	inline void InputManager::InvokeInput(InputType input, Data state)
	{
		InvokeInputImpl(std::move(input), std::move(state));
	}

}

namespace tei::external
{
	constexpr static auto const& Input{ tei::internal::input::Input };
}