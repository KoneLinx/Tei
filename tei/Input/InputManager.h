#pragma once

#include <vector>
#include <memory>
#include <tuple>
#include <typeindex>

#include "InputType.h"
#include "Command.h"

#include <tei/utility.h>

namespace tei::internal::input
{

	class InputManager
	{

		class CommandHandle;

	public:

		InputManager();
		~InputManager();

		template <typename InputType>
		[[nodiscard]] CommandHandle AddCommand(Command<InputType> command = {});
		
		template <typename InputType, typename Action> requires std::constructible_from<Command<InputType>, InputType, Action>
		[[nodiscard]] CommandHandle AddCommand(InputType input, Action action);

		static void RemoveCommand(CommandHandle&);

		void ProcessInput();

		bool IsPressed(InputBinary const& button) const;

		template <typename InputType>
		typename InputType::Data GetInputData(InputType input) const;

		template <typename InputType, typename Data = typename InputType::Data>
		void InvokeInput(InputType input, Data state);

		struct PollData;

	private:

		using CommandContainer = std::unordered_multimap<std::type_index, std::any>;

		CommandHandle AddCommand(CommandContainer::value_type);

		SomeCommonInputData GetInputImpl(SomeCommonInputTypeRef) const;
		void InvokeInputImpl(SomeCommonInputTypeRef, SomeCommonInputDataRef) const;

		std::shared_ptr<CommandContainer> m_Commands;

		std::unique_ptr<PollData> m_PollData;

		class CommandHandle
		{
		public:

			CommandHandle() = default;
			~CommandHandle();
			
			CommandHandle(CommandHandle &&) = default;
			CommandHandle& operator = (CommandHandle &&);

			CommandHandle(CommandHandle const&) = delete;
			CommandHandle& operator = (CommandHandle const&) = delete;

			bool Alive();
			void Clear();
			void Detach();

		private:

			friend InputManager;
			
			CommandHandle(std::weak_ptr<CommandContainer>, CommandContainer::iterator);

			std::weak_ptr<CommandContainer> m_Container{};
			CommandContainer::iterator m_Position{};
		};

	public:

		using Service = utility::Service<InputManager>;

	};

	extern InputManager::Service Input;

	template <typename InputType>
	inline InputManager::CommandHandle InputManager::AddCommand(Command<InputType> command)
	{
		return AddCommand({
			typeid(InputType),
			std::move(command) 
		});
	}

	template <typename InputType, typename Action> requires std::constructible_from<Command<InputType>, InputType, Action>
	inline InputManager::CommandHandle InputManager::AddCommand(InputType input, Action action)
	{
		return AddCommand(Command<InputType>{ input, std::move(action) });
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